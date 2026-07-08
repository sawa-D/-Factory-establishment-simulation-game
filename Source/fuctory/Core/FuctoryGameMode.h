#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FuctoryGameMode.generated.h"

class UPhaseData;

UCLASS()
class FUCTORY_API AFuctoryGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AFuctoryGameMode();
    virtual void BeginPlay() override;

    // BP で各フェーズのデータアセットをアサイン
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game|Phase")
    TArray<TObjectPtr<UPhaseData>> PhaseDataAssets;

    // 開始資金
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game|Economy")
    float InitialMoney = 50000000.f;

private:
    void InitializeGame();
};
