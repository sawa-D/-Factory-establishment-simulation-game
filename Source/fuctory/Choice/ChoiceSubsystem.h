#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/FuctoryGameTypes.h"
#include "ChoiceSubsystem.generated.h"

class UChoiceData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoicePresented, UChoiceData*, Choice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOptionSelected, UChoiceData*, Choice, int32, OptionIndex);

UCLASS(BlueprintType)
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

    // 選択結果の説明用に、FChoiceOptionの非ゼロの変化のみをテキスト化する(UI用)
    UFUNCTION(BlueprintPure, Category = "Choice")
    static FText GetOptionChangeSummary(const FChoiceOption& Option);

    // 選択肢の数値(MoneyCost/DaysUsed/各Delta)に、プレイスルー単位のランダム変動(±10%)を適用した結果を返す。
    // 表示(結果パネル)と適用(ステータス反映)の両方でこの関数を経由することで数値を一致させる。
    UFUNCTION(BlueprintPure, Category = "Choice")
    FChoiceOption GetVariedOption(UChoiceData* Choice, int32 OptionIndex) const;

    // 新しいプレイスルー開始時に呼ぶ(ランダム変動のシードを再抽選)
    UFUNCTION(BlueprintCallable, Category = "Choice")
    void ResetForNewPlaythrough();

    // フェーズ切り替え時に完了履歴をリセット
    UFUNCTION(BlueprintCallable, Category = "Choice")
    void ResetForPhase();

    UPROPERTY(BlueprintAssignable, Category = "Choice")
    FOnChoicePresented OnChoicePresented;

    UPROPERTY(BlueprintAssignable, Category = "Choice")
    FOnOptionSelected OnOptionSelected;

private:
    TSet<FName> CompletedChoiceNames;
    int32 PlaythroughSeed = 0;
};
