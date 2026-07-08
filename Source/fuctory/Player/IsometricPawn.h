#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IsometricPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class FUCTORY_API AIsometricPawn : public APawn
{
    GENERATED_BODY()

public:
    AIsometricPawn();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> Camera;

    // カメラ設定
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MoveSpeed = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float ZoomSpeed = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MinArmLength = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float MaxArmLength = 3000.f;

private:
    void MoveRight(float Value);
    void MoveForward(float Value);
    void ZoomIn(float Value);

    FVector MoveInput;
    float ZoomInput = 0.f;
};
