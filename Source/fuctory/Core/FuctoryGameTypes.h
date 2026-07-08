#pragma once

#include "CoreMinimal.h"
#include "FuctoryGameTypes.generated.h"

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
    None            UMETA(DisplayName = "None"),
    Construction    UMETA(DisplayName = "工場建屋建設期"),
    Setup           UMETA(DisplayName = "生産ライン立ち上げ期"),
    MassProduction  UMETA(DisplayName = "量産期"),
    GameOver        UMETA(DisplayName = "ゲームオーバー"),
    GameClear       UMETA(DisplayName = "ゲームクリア")
};

UENUM(BlueprintType)
enum class EGameOverReason : uint8
{
    None                UMETA(DisplayName = "None"),
    Bankruptcy          UMETA(DisplayName = "資金不足"),
    LowBuildingQuality  UMETA(DisplayName = "建屋品質不足"),
    LowEquipmentQuality UMETA(DisplayName = "設備品質不足"),
    LowMaterialQuality  UMETA(DisplayName = "資材品質基準未達"),
    ProductionFailure   UMETA(DisplayName = "生産目標未達"),
    DeliveryFailure     UMETA(DisplayName = "納期遅延"),
    ReputationCollapse  UMETA(DisplayName = "信用失墜"),
    TimeOver            UMETA(DisplayName = "期限超過")
};

// 工場の全統計値。選択肢の効果で増減し、隠れ条件チェックに使われる
USTRUCT(BlueprintType)
struct FFactoryStats
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy")
    float Money = 50000000.f;           // 資金(円)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
    float BuildingQuality = 0.f;        // 建屋品質 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Construction")
    float EquipmentQuality = 0.f;       // 設備品質 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    float MaterialQuality = 0.f;        // 資材品質 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    float MaterialStock = 0.f;          // 資材在庫量(単位)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HR")
    float WorkerSkill = 0.f;            // 作業員技術力 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HR")
    int32 WorkerCount = 0;              // 作業員数

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
    float ProductionEfficiency = 0.f;   // 生産効率 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Production")
    float ProductQuality = 0.f;         // 製品品質 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sales")
    float CustomerSatisfaction = 70.f;  // 顧客満足度 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sales")
    float Reputation = 70.f;           // 信用評価 0-100

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
    int32 DaysUsed = 0;                 // フェーズ内経過日数
};

// 選択肢の1オプション
USTRUCT(BlueprintType)
struct FChoiceOption
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
    FText OptionText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
    FText OptionDetail;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display")
    FText ResultText;

    // --- コスト ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost")
    float MoneyCost = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cost")
    int32 DaysUsed = 0;

    // --- ステータス変化量 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float BuildingQualityDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float EquipmentQualityDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float MaterialQualityDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float MaterialStockDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float WorkerSkillDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    int32 WorkerCountDelta = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float ProductionEfficiencyDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float ProductQualityDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float CustomerSatisfactionDelta = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float ReputationDelta = 0.f;

    // 資材品質・数量を直接設定する選択肢（調達系）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    bool bOverrideMaterialQuality = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
    float MaterialQualityOverride = 0.f;
};

// フェーズクリア目標
USTRUCT(BlueprintType)
struct FPhaseObjective
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
    FText ObjectiveText;

    // FFactoryStats のプロパティ名 (例: "BuildingQuality")
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
    FName StatName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
    float RequiredValue = 0.f;

    // true = 以上, false = 以下
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Objective")
    bool bMinRequired = true;
};

// 隠れゲームオーバー条件
USTRUCT(BlueprintType)
struct FHiddenCondition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
    FName StatName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
    float MinValue = -1.f;   // -1 = チェックなし

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
    float MaxValue = -1.f;   // -1 = チェックなし

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
    EGameOverReason Reason = EGameOverReason::None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
    FText FailureMessage;
};
