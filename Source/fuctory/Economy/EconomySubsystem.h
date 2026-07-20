#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EconomySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, float, NewAmount);

UCLASS(BlueprintType)
class FUCTORY_API UEconomySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // 支出。残高不足なら false を返す
    UFUNCTION(BlueprintCallable, Category = "Economy")
    bool SpendMoney(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void AddMoney(float Amount);

    UFUNCTION(BlueprintPure, Category = "Economy")
    float GetMoney() const { return CurrentMoney; }

    UFUNCTION(BlueprintPure, Category = "Economy")
    bool CanAfford(float Amount) const { return CurrentMoney >= Amount; }

    UFUNCTION(BlueprintCallable, Category = "Economy")
    void SetInitialMoney(float Amount);

    UPROPERTY(BlueprintAssignable, Category = "Economy")
    FOnMoneyChanged OnMoneyChanged;

private:
    float CurrentMoney = 50000000.f;
};
