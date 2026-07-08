#include "PhaseSubsystem.h"
#include "PhaseData.h"
#include "Economy/EconomySubsystem.h"

void UPhaseSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UPhaseSubsystem::StartGame(const TArray<UPhaseData*>& InPhaseDataAssets, float InitialMoney)
{
    AllPhaseData = InPhaseDataAssets;
    Stats = FFactoryStats();
    Stats.Money = InitialMoney;
    bGameEnded = false;

    if (UEconomySubsystem* Eco = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
    {
        Eco->SetInitialMoney(InitialMoney);
    }

    CurrentPhaseIndex = -1;
    EnterPhase(0);
}

void UPhaseSubsystem::EnterPhase(int32 PhaseIndex)
{
    if (!AllPhaseData.IsValidIndex(PhaseIndex)) return;

    CurrentPhaseIndex = PhaseIndex;
    CurrentPhase = AllPhaseData[PhaseIndex]->Phase;
    Stats.DaysUsed = 0;

    OnPhaseChanged.Broadcast(CurrentPhase);
}

void UPhaseSubsystem::ApplyChoiceOption(const FChoiceOption& Option)
{
    if (bGameEnded) return;

    // 資金消費
    if (Option.MoneyCost > 0.f)
    {
        if (UEconomySubsystem* Eco = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
        {
            if (!Eco->SpendMoney(Option.MoneyCost))
            {
                OnGameOver.Broadcast(
                    EGameOverReason::Bankruptcy,
                    FText::FromString(TEXT("資金が不足しています。これ以上の投資ができません。")));
                CurrentPhase = EGamePhase::GameOver;
                bGameEnded = true;
                return;
            }
            Stats.Money = Eco->GetMoney();
        }
    }

    // ステータス反映
    Stats.BuildingQuality       = FMath::Clamp(Stats.BuildingQuality       + Option.BuildingQualityDelta,       0.f, 100.f);
    Stats.EquipmentQuality      = FMath::Clamp(Stats.EquipmentQuality      + Option.EquipmentQualityDelta,      0.f, 100.f);
    Stats.MaterialQuality       = FMath::Clamp(Stats.MaterialQuality       + Option.MaterialQualityDelta,       0.f, 100.f);
    Stats.MaterialStock         = FMath::Max(0.f, Stats.MaterialStock      + Option.MaterialStockDelta);
    Stats.WorkerSkill           = FMath::Clamp(Stats.WorkerSkill           + Option.WorkerSkillDelta,           0.f, 100.f);
    Stats.WorkerCount           = FMath::Max(0,   Stats.WorkerCount        + Option.WorkerCountDelta);
    Stats.ProductionEfficiency  = FMath::Clamp(Stats.ProductionEfficiency  + Option.ProductionEfficiencyDelta,  0.f, 100.f);
    Stats.ProductQuality        = FMath::Clamp(Stats.ProductQuality        + Option.ProductQualityDelta,        0.f, 100.f);
    Stats.CustomerSatisfaction  = FMath::Clamp(Stats.CustomerSatisfaction  + Option.CustomerSatisfactionDelta,  0.f, 100.f);
    Stats.Reputation            = FMath::Clamp(Stats.Reputation            + Option.ReputationDelta,            0.f, 100.f);
    Stats.DaysUsed             += Option.DaysUsed;

    if (Option.bOverrideMaterialQuality)
    {
        Stats.MaterialQuality = FMath::Clamp(Option.MaterialQualityOverride, 0.f, 100.f);
    }

    OnStatsUpdated.Broadcast(Stats);
    CheckHiddenConditions();
}

void UPhaseSubsystem::CheckHiddenConditions()
{
    if (bGameEnded) return;

    UPhaseData* PhaseData = GetCurrentPhaseData();
    if (!PhaseData) return;

    // 期限チェック
    if (Stats.DaysUsed > PhaseData->DayLimit)
    {
        OnGameOver.Broadcast(
            EGameOverReason::TimeOver,
            FText::Format(FText::FromString(TEXT("{0} の期限({1}日)を超過しました。")),
                PhaseData->PhaseName, FText::AsNumber(PhaseData->DayLimit)));
        CurrentPhase = EGamePhase::GameOver;
        bGameEnded = true;
        return;
    }

    // 隠れ条件チェック
    for (const FHiddenCondition& Cond : PhaseData->HiddenConditions)
    {
        if (!EvaluateCondition(Cond))
        {
            OnGameOver.Broadcast(Cond.Reason, Cond.FailureMessage);
            CurrentPhase = EGamePhase::GameOver;
            bGameEnded = true;
            return;
        }
    }
}

void UPhaseSubsystem::TryAdvancePhase()
{
    if (bGameEnded) return;
    if (!AreObjectivesMet()) return;

    UPhaseData* PhaseData = GetCurrentPhaseData();
    if (PhaseData && PhaseData->CompletionRevenue > 0.f)
    {
        if (UEconomySubsystem* Eco = GetGameInstance()->GetSubsystem<UEconomySubsystem>())
        {
            Eco->AddMoney(PhaseData->CompletionRevenue);
            Stats.Money = Eco->GetMoney();
        }
    }

    int32 NextIndex = CurrentPhaseIndex + 1;
    if (!AllPhaseData.IsValidIndex(NextIndex))
    {
        CurrentPhase = EGamePhase::GameClear;
        OnGameClear.Broadcast();
    }
    else
    {
        EnterPhase(NextIndex);
    }
}

bool UPhaseSubsystem::AreObjectivesMet() const
{
    UPhaseData* PhaseData = GetCurrentPhaseData();
    if (!PhaseData) return false;

    for (const FPhaseObjective& Obj : PhaseData->Objectives)
    {
        float Val = GetStatValue(Obj.StatName);
        if (Obj.bMinRequired && Val < Obj.RequiredValue) return false;
        if (!Obj.bMinRequired && Val > Obj.RequiredValue) return false;
    }
    return true;
}

void UPhaseSubsystem::GetObjectiveStatus(TArray<FPhaseObjective>& OutObjectives, TArray<bool>& OutMet) const
{
    OutObjectives.Empty();
    OutMet.Empty();

    UPhaseData* PhaseData = GetCurrentPhaseData();
    if (!PhaseData) return;

    for (const FPhaseObjective& Obj : PhaseData->Objectives)
    {
        float Val = GetStatValue(Obj.StatName);
        bool bMet = Obj.bMinRequired ? (Val >= Obj.RequiredValue) : (Val <= Obj.RequiredValue);
        OutObjectives.Add(Obj);
        OutMet.Add(bMet);
    }
}

UPhaseData* UPhaseSubsystem::GetCurrentPhaseData() const
{
    if (AllPhaseData.IsValidIndex(CurrentPhaseIndex))
        return AllPhaseData[CurrentPhaseIndex];
    return nullptr;
}

int32 UPhaseSubsystem::GetRemainingDays() const
{
    UPhaseData* PhaseData = GetCurrentPhaseData();
    if (!PhaseData) return 0;
    return FMath::Max(0, PhaseData->DayLimit - Stats.DaysUsed);
}

float UPhaseSubsystem::GetStatValue(const FName& StatName) const
{
    if (StatName == "Money")                return Stats.Money;
    if (StatName == "BuildingQuality")      return Stats.BuildingQuality;
    if (StatName == "EquipmentQuality")     return Stats.EquipmentQuality;
    if (StatName == "MaterialQuality")      return Stats.MaterialQuality;
    if (StatName == "MaterialStock")        return Stats.MaterialStock;
    if (StatName == "WorkerSkill")          return Stats.WorkerSkill;
    if (StatName == "WorkerCount")          return (float)Stats.WorkerCount;
    if (StatName == "ProductionEfficiency") return Stats.ProductionEfficiency;
    if (StatName == "ProductQuality")       return Stats.ProductQuality;
    if (StatName == "CustomerSatisfaction") return Stats.CustomerSatisfaction;
    if (StatName == "Reputation")           return Stats.Reputation;
    if (StatName == "DaysUsed")             return (float)Stats.DaysUsed;
    return 0.f;
}

bool UPhaseSubsystem::EvaluateCondition(const FHiddenCondition& Condition) const
{
    float Val = GetStatValue(Condition.StatName);
    if (Condition.MinValue >= 0.f && Val < Condition.MinValue) return false;
    if (Condition.MaxValue >= 0.f && Val > Condition.MaxValue) return false;
    return true;
}
