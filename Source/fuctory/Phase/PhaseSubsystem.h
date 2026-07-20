#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Core/FuctoryGameTypes.h"
#include "PhaseSubsystem.generated.h"

class UPhaseData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EGamePhase, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameOver, EGameOverReason, Reason, const FText&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameClear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatsUpdated, const FFactoryStats&, Stats);

UCLASS(BlueprintType)
class FUCTORY_API UPhaseSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // ゲーム開始。フェーズデータアセット配列を渡す
    UFUNCTION(BlueprintCallable, Category = "Phase")
    void StartGame(const TArray<UPhaseData*>& InPhaseDataAssets, float InitialMoney);

    // 現在フェーズの全必須選択が完了したら次フェーズへ
    UFUNCTION(BlueprintCallable, Category = "Phase")
    void TryAdvancePhase();

    // 選択肢の効果をステータスに適用し、隠れ条件をチェックする
    UFUNCTION(BlueprintCallable, Category = "Phase")
    void ApplyChoiceOption(const FChoiceOption& Option);

    // フェーズクリア目標の達成状況
    UFUNCTION(BlueprintPure, Category = "Phase")
    bool AreObjectivesMet() const;

    // 現在フェーズの目標テキストと達成状況を返す (BP用)
    UFUNCTION(BlueprintPure, Category = "Phase")
    void GetObjectiveStatus(TArray<FPhaseObjective>& OutObjectives, TArray<bool>& OutMet) const;

    UFUNCTION(BlueprintPure, Category = "Phase")
    EGamePhase GetCurrentPhase() const { return CurrentPhase; }

    UFUNCTION(BlueprintPure, Category = "Phase")
    UPhaseData* GetCurrentPhaseData() const;

    UFUNCTION(BlueprintPure, Category = "Phase")
    const FFactoryStats& GetStats() const { return Stats; }

    UFUNCTION(BlueprintPure, Category = "Phase")
    int32 GetRemainingDays() const;

    UPROPERTY(BlueprintAssignable, Category = "Phase")
    FOnPhaseChanged OnPhaseChanged;

    UPROPERTY(BlueprintAssignable, Category = "Phase")
    FOnGameOver OnGameOver;

    UPROPERTY(BlueprintAssignable, Category = "Phase")
    FOnGameClear OnGameClear;

    UPROPERTY(BlueprintAssignable, Category = "Phase")
    FOnStatsUpdated OnStatsUpdated;

private:
    void EnterPhase(int32 PhaseIndex);
    void CheckHiddenConditions();
    float GetStatValue(const FName& StatName) const;
    bool EvaluateCondition(const FHiddenCondition& Condition) const;

    EGamePhase CurrentPhase = EGamePhase::None;
    int32 CurrentPhaseIndex = -1;
    TArray<TObjectPtr<UPhaseData>> AllPhaseData;
    FFactoryStats Stats;
    bool bGameEnded = false;
};
