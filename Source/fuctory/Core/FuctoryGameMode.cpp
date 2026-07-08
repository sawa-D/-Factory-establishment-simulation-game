#include "FuctoryGameMode.h"
#include "Phase/PhaseSubsystem.h"
#include "Phase/PhaseData.h"
#include "Player/FuctoryPlayerController.h"
#include "Player/IsometricPawn.h"

AFuctoryGameMode::AFuctoryGameMode()
{
    PlayerControllerClass = AFuctoryPlayerController::StaticClass();
    DefaultPawnClass      = AIsometricPawn::StaticClass();
}

void AFuctoryGameMode::BeginPlay()
{
    Super::BeginPlay();
    InitializeGame();
}

void AFuctoryGameMode::InitializeGame()
{
    UGameInstance* GI = GetGameInstance();
    if (!GI) return;

    TArray<UPhaseData*> DataArray;
    for (UPhaseData* PD : PhaseDataAssets)
    {
        if (PD) DataArray.Add(PD);
    }

    if (UPhaseSubsystem* PhaseSys = GI->GetSubsystem<UPhaseSubsystem>())
    {
        PhaseSys->StartGame(DataArray, InitialMoney);
    }
}
