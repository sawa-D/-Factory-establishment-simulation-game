#include "IsometricPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

AIsometricPawn::AIsometricPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(SceneRoot);
    SpringArm->TargetArmLength = 1500.f;
    SpringArm->SetRelativeRotation(FRotator(-55.f, 45.f, 0.f)); // アイソメ角度
    SpringArm->bDoCollisionTest = false;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 8.f;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->SetProjectionMode(ECameraProjectionMode::Perspective);
    Camera->FieldOfView = 60.f;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw   = false;
    bUseControllerRotationRoll  = false;
}

void AIsometricPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveRight",   this, &AIsometricPawn::MoveRight);
    PlayerInputComponent->BindAxis("MoveForward", this, &AIsometricPawn::MoveForward);
    PlayerInputComponent->BindAxis("Zoom",        this, &AIsometricPawn::ZoomIn);
}

void AIsometricPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!MoveInput.IsNearlyZero())
    {
        FVector Delta = MoveInput * MoveSpeed * DeltaTime;
        AddActorWorldOffset(Delta);
        MoveInput = FVector::ZeroVector;
    }

    if (FMath::Abs(ZoomInput) > SMALL_NUMBER)
    {
        float NewLength = FMath::Clamp(
            SpringArm->TargetArmLength - ZoomInput * ZoomSpeed * DeltaTime,
            MinArmLength, MaxArmLength);
        SpringArm->TargetArmLength = NewLength;
        ZoomInput = 0.f;
    }
}

void AIsometricPawn::MoveRight(float Value)
{
    MoveInput.Y += Value;
}

void AIsometricPawn::MoveForward(float Value)
{
    MoveInput.X += Value;
}

void AIsometricPawn::ZoomIn(float Value)
{
    ZoomInput = Value;
}
