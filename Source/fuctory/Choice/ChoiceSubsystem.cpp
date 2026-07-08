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

    // PhaseSubsystem にステータス変化を適用
    if (UPhaseSubsystem* PhaseSys = GetGameInstance()->GetSubsystem<UPhaseSubsystem>())
    {
        PhaseSys->ApplyChoiceOption(Option);
    }

    // 完了済みとしてマーク
    CompletedChoiceNames.Add(Choice->GetFName());

    OnOptionSelected.Broadcast(Choice, OptionIndex);
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
