#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/FuctoryGameTypes.h"
#include "FuctoryPlayerController.generated.h"

class UChoiceData;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class FUCTORY_API AFuctoryPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AFuctoryPlayerController();
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // Enhanced Input: テスト用にフェーズ進行と選択肢提示をキー入力から呼べるようにする
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_PresentChoice;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> IA_CompletePhase;

    // ChoiceSubsystem からの通知を受けて BP の UI を呼び出す
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowChoice(UChoiceData* Choice);

    // フェーズ切り替え通知 → BP で演出
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void BP_OnPhaseChanged(EGamePhase NewPhase);

    // 選択肢UIの表示 → BP で Widget を開く
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void BP_ShowChoiceDialog(UChoiceData* Choice);

    // ゲームオーバー → BP で画面表示
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void BP_OnGameOver(EGameOverReason Reason, const FText& Message);

    // ゲームクリア → BP で画面表示
    UFUNCTION(BlueprintImplementableEvent, Category = "UI")
    void BP_OnGameClear();

    // 選択肢オプションを選んだ時に BP から呼ぶ
    UFUNCTION(BlueprintCallable, Category = "Choice")
    void SelectChoiceOption(UChoiceData* Choice, int32 OptionIndex);

    // 現在フェーズの次の未完了選択肢を提示する
    UFUNCTION(BlueprintCallable, Category = "Choice")
    void PresentNextChoice();

    // フェーズ完了を試みる
    UFUNCTION(BlueprintCallable, Category = "Phase")
    void TryCompletePhase();

private:
    UFUNCTION()
    void OnPhaseChanged(EGamePhase NewPhase);

    UFUNCTION()
    void OnGameOver(EGameOverReason Reason, const FText& Message);

    UFUNCTION()
    void OnGameClear();

    void OnInputPresentChoice(const FInputActionValue& Value);
    void OnInputCompletePhase(const FInputActionValue& Value);
};
