#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/FuctoryGameTypes.h"
#include "ChoiceData.generated.h"

UCLASS(BlueprintType)
class FUCTORY_API UChoiceData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
    FText ChoiceTitle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
    FText ChoiceDescription;

    // この選択肢が属するフェーズ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
    EGamePhase Phase = EGamePhase::None;

    // フェーズ内での表示順
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
    int32 ChoiceOrder = 0;

    // true の場合、フェーズクリアに必須
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
    bool bIsRequired = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Choice")
    TArray<FChoiceOption> Options;
};
