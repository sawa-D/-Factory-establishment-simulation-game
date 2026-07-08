#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/FuctoryGameTypes.h"
#include "ChoiceSubsystem.generated.h"

class UChoiceData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicePresented, UChoiceData*, Choice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOptionSelected, UChoiceData*, Choice, int32, OptionIndex);

UCLASS()
class FUCTORY_API UChoiceSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 選択肢を提示する(UIに通知)
    UFUNCTION(BlueprintCallable, Category = "Choice")
    void PresentChoice(UChoiceData* Choice);

    // プレイヤーがオプションを選択した際に呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Choice")
    void SelectOption(UChoiceData* Choice, int32 OptionIndex);

    // 現在のフェーズで未完了の選択肢を返す
    UFUNCTION(BlueprintPure, Category = "Choice")
    TArray<UChoiceData*> GetPendingChoices(EGamePhase Phase, const TArray<UChoiceData*>& AllChoices) const;

    // 全必須選択肢が完了しているか
    UFUNCTION(BlueprintPure, Category = "Choice")
    bool AreRequiredChoicesCompleted(const TArray<UChoiceData*>& RequiredChoices) const;

    UFUNCTION(BlueprintPure, Category = "Choice")
    bool IsChoiceCompleted(UChoiceData* Choice) const;

    // フェーズ切り替え時に完了履歴をリセット
    UFUNCTION(BlueprintCallable, Category = "Choice")
    void ResetForPhase();

    UPROPERTY(BlueprintAssignable, Category = "Choice")
    FOnChoicePresented OnChoicePresented;

    UPROPERTY(BlueprintAssignable, Category = "Choice")
    FOnOptionSelected OnOptionSelected;

private:
    TSet<FName> CompletedChoiceNames;
};
