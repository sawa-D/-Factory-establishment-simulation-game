#include "FuctoryPlayerController.h"
#include "Phase/PhaseSubsystem.h"
#include "Phase/PhaseData.h"
#include "Choice/ChoiceSubsystem.h"
#include "Choice/ChoiceData.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"

AFuctoryPlayerController::AFuctoryPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
}

void AFuctoryPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UGameInstance* GI = GetGameInstance();
    if (!GI) return;

    if (UPhaseSubsystem* PhaseSys = GI->GetSubsystem<UPhaseSubsystem>())
    {
        PhaseSys->OnPhaseChanged.AddDynamic(this, &AFuctoryPlayerController::OnPhaseChanged);
        PhaseSys->OnGameOver.AddDynamic(this, &AFuctoryPlayerController::OnGameOver);
        PhaseSys->OnGameClear.AddDynamic(this, &AFuctoryPlayerController::OnGameClear);

        // GameMode::BeginPlay は PlayerController::BeginPlay より先に実行されるため、
        // ここに登録が完了する前に最初の OnPhaseChanged が発火してしまっている場合がある。
        // その場合は現在フェーズを手動で通知して同期する。
        if (PhaseSys->GetCurrentPhase() != EGamePhase::None)
        {
            OnPhaseChanged(PhaseSys->GetCurrentPhase());
        }
    }

    if (ULocalPlayer* LP = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void AFuctoryPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (IA_PresentChoice)
        {
            EIC->BindAction(IA_PresentChoice, ETriggerEvent::Started, this, &AFuctoryPlayerController::OnInputPresentChoice);
        }
        if (IA_CompletePhase)
        {
            EIC->BindAction(IA_CompletePhase, ETriggerEvent::Started, this, &AFuctoryPlayerController::OnInputCompletePhase);
        }
    }
}

void AFuctoryPlayerController::OnInputPresentChoice(const FInputActionValue& Value)
{
    PresentNextChoice();
}

void AFuctoryPlayerController::OnInputCompletePhase(const FInputActionValue& Value)
{
    TryCompletePhase();
}

void AFuctoryPlayerController::ShowChoice(UChoiceData* Choice)
{
    if (!Choice) return;
    BP_ShowChoiceDialog(Choice);
}

void AFuctoryPlayerController::SelectChoiceOption(UChoiceData* Choice, int32 OptionIndex)
{
    UGameInstance* GI = GetGameInstance();
    if (!GI) return;

    if (UChoiceSubsystem* ChoiceSys = GI->GetSubsystem<UChoiceSubsystem>())
    {
        ChoiceSys->SelectOption(Choice, OptionIndex);
    }
}

void AFuctoryPlayerController::PresentNextChoice()
{
    UGameInstance* GI = GetGameInstance();
    if (!GI) return;

    UPhaseSubsystem* PhaseSys  = GI->GetSubsystem<UPhaseSubsystem>();
    UChoiceSubsystem* ChoiceSys = GI->GetSubsystem<UChoiceSubsystem>();
    if (!PhaseSys || !ChoiceSys) return;

    UPhaseData* PhaseData = PhaseSys->GetCurrentPhaseData();
    if (!PhaseData) return;

    TArray<UChoiceData*> RawChoices;
    for (UChoiceData* C : PhaseData->AvailableChoices) RawChoices.Add(C);

    TArray<UChoiceData*> Pending = ChoiceSys->GetPendingChoices(PhaseSys->GetCurrentPhase(), RawChoices);
    if (Pending.Num() > 0)
    {
        ChoiceSys->PresentChoice(Pending[0]);
        BP_ShowChoiceDialog(Pending[0]);
    }
}

void AFuctoryPlayerController::TryCompletePhase()
{
    UGameInstance* GI = GetGameInstance();
    if (!GI) return;

    if (UPhaseSubsystem* PhaseSys = GI->GetSubsystem<UPhaseSubsystem>())
    {
        PhaseSys->TryAdvancePhase();
    }
}

void AFuctoryPlayerController::OnPhaseChanged(EGamePhase NewPhase)
{
    if (UChoiceSubsystem* ChoiceSys = GetGameInstance()->GetSubsystem<UChoiceSubsystem>())
    {
        ChoiceSys->ResetForPhase();
    }
    BP_OnPhaseChanged(NewPhase);
}

void AFuctoryPlayerController::OnGameOver(EGameOverReason Reason, const FText& Message)
{
    BP_OnGameOver(Reason, Message);
}

void AFuctoryPlayerController::OnGameClear()
{
    BP_OnGameClear();
}
