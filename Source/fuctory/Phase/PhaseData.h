#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/FuctoryGameTypes.h"
#include "PhaseData.generated.h"

class UChoiceData;

UCLASS(BlueprintType)
class FUCTORY_API UPhaseData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    EGamePhase Phase = EGamePhase::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    FText PhaseName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    FText PhaseDescription;

    // フェーズの制限日数
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    int32 DayLimit = 90;

    // このフェーズで提示される選択肢一覧
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    TArray<TObjectPtr<UChoiceData>> AvailableChoices;

    // フェーズクリアに必要な目標
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    TArray<FPhaseObjective> Objectives;

    // 選択後に毎回チェックされる隠れゲームオーバー条件
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    TArray<FHiddenCondition> HiddenConditions;

    // フェーズクリア時の報酬(受注金など)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phase")
    float CompletionRevenue = 0.f;
};
