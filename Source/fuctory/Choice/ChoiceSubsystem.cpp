#include "ChoiceSubsystem.h"
#include "ChoiceData.h"
#include "Phase/PhaseSubsystem.h"

void UChoiceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UChoiceSubsystem::PresentChoice(UChoiceData* Choice)
{
    if (!Choice) return;
    OnChoicePresented.Broadcast(Choice);
}

void UChoiceSubsystem::SelectOption(UChoiceData* Choice, int32 OptionIndex)
{
    if (!Choice || !Choice->Options.IsValidIndex(OptionIndex)) return;

    const FChoiceOption& Option = Choice->Options[OptionIndex];

    // 完了済みとして先にマークする(PhaseSubsystem側の「選択肢を使い切ったか」判定が
    // 今回選んだ選択肢を除外して正しく評価できるようにするため)
    CompletedChoiceNames.Add(Choice->GetFName());

    // PhaseSubsystem にステータス変化を適用
    if (UPhaseSubsystem* PhaseSys = GetGameInstance()->GetSubsystem<UPhaseSubsystem>())
    {
        PhaseSys->ApplyChoiceOption(Option);
    }

    OnOptionSelected.Broadcast(Choice, OptionIndex);
}

FText UChoiceSubsystem::GetOptionChangeSummary(const FChoiceOption& Option)
{
    TArray<FString> Lines;

    if (Option.MoneyCost != 0.f)
    {
        Lines.Add(FString::Printf(TEXT("資金: -%s円"), *FText::AsNumber(Option.MoneyCost).ToString()));
    }
    if (Option.DaysUsed != 0)
    {
        Lines.Add(FString::Printf(TEXT("残り日数: -%d日"), Option.DaysUsed));
    }

    auto AddDelta = [&Lines](const TCHAR* Label, float Delta)
    {
        if (Delta != 0.f)
        {
            Lines.Add(FString::Printf(TEXT("%s: %s%.0f"), Label, Delta > 0.f ? TEXT("+") : TEXT(""), Delta));
        }
    };

    AddDelta(TEXT("建屋品質"), Option.BuildingQualityDelta);
    AddDelta(TEXT("設備品質"), Option.EquipmentQualityDelta);
    AddDelta(TEXT("資材品質"), Option.MaterialQualityDelta);
    AddDelta(TEXT("資材在庫"), Option.MaterialStockDelta);
    AddDelta(TEXT("作業員技術力"), Option.WorkerSkillDelta);

    if (Option.WorkerCountDelta != 0)
    {
        Lines.Add(FString::Printf(TEXT("作業員数: %s%d"), Option.WorkerCountDelta > 0 ? TEXT("+") : TEXT(""), Option.WorkerCountDelta));
    }

    AddDelta(TEXT("生産効率"), Option.ProductionEfficiencyDelta);
    AddDelta(TEXT("製品品質"), Option.ProductQualityDelta);
    AddDelta(TEXT("顧客満足度"), Option.CustomerSatisfactionDelta);
    AddDelta(TEXT("信用評価"), Option.ReputationDelta);
    AddDelta(TEXT("システム導入進捗"), Option.SystemProgressDelta);

    if (Option.bOverrideMaterialQuality)
    {
        Lines.Add(FString::Printf(TEXT("資材品質: %.0fに設定"), Option.MaterialQualityOverride));
    }

    return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

TArray<UChoiceData*> UChoiceSubsystem::GetPendingChoices(EGamePhase Phase, const TArray<UChoiceData*>& AllChoices) const
{
    TArray<UChoiceData*> Pending;
    for (UChoiceData* Choice : AllChoices)
    {
        if (Choice && Choice->Phase == Phase && !CompletedChoiceNames.Contains(Choice->GetFName()))
        {
            Pending.Add(Choice);
        }
    }
    // ChoiceOrder 順にソート
    Pending.Sort([](const UChoiceData& A, const UChoiceData& B)
    {
        return A.ChoiceOrder < B.ChoiceOrder;
    });
    return Pending;
}

bool UChoiceSubsystem::AreRequiredChoicesCompleted(const TArray<UChoiceData*>& RequiredChoices) const
{
    for (UChoiceData* Choice : RequiredChoices)
    {
        if (Choice && Choice->bIsRequired && !CompletedChoiceNames.Contains(Choice->GetFName()))
        {
            return false;
        }
    }
    return true;
}

bool UChoiceSubsystem::IsChoiceCompleted(UChoiceData* Choice) const
{
    if (!Choice) return false;
    return CompletedChoiceNames.Contains(Choice->GetFName());
}

void UChoiceSubsystem::ResetForPhase()
{
    CompletedChoiceNames.Empty();
}
