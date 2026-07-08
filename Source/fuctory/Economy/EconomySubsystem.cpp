#include "EconomySubsystem.h"

void UEconomySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    CurrentMoney = 50000000.f;
}

bool UEconomySubsystem::SpendMoney(float Amount)
{
    if (Amount <= 0.f) return true;
    if (CurrentMoney < Amount) return false;

    CurrentMoney -= Amount;
    OnMoneyChanged.Broadcast(CurrentMoney);
    return true;
}

void UEconomySubsystem::AddMoney(float Amount)
{
    if (Amount <= 0.f) return;
    CurrentMoney += Amount;
    OnMoneyChanged.Broadcast(CurrentMoney);
}

void UEconomySubsystem::SetInitialMoney(float Amount)
{
    CurrentMoney = FMath::Max(0.f, Amount);
    OnMoneyChanged.Broadcast(CurrentMoney);
}
