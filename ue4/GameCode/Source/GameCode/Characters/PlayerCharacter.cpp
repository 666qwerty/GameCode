// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/GCBaseCharacterMovementComponent.h"
#include "Components/CharacterEquipmentComponent.h"
#include "Components/CharacterAttributesComponent.h"
#include "Equipment/Weapons/RangeWeaponItem.h"
#include "Camera/CameraComponent.h"
#include "Controllers/GCPlayerController.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->bUsePawnControlRotation = true;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComponent->SetupAttachment(SpringArmComponent);
    CameraComponent->bUsePawnControlRotation = false;

    GetCharacterMovement()->RotationRate = FRotator(0,540,0);
    GetCharacterMovement()->bOrientRotationToMovement = 1;

    PrimaryActorTick.bCanEverTick = true;
    Team = ETeams::Player;
    //SetReplicates(true);
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    PlayerController = Cast<AGCPlayerController>(Controller);
    if (IsValid(AimTimelineCurve))
    {
        FOnTimelineFloatStatic AimTimelineUpdate;
        AimTimelineUpdate.BindUObject(this, &APlayerCharacter::AimTimelineUpdate);
        AimTimeline.AddInterpFloat(AimTimelineCurve, AimTimelineUpdate);
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    float AimFOVSpeed = GetGCEquipmentComponent()->GetCurrentRangeWeapon() != nullptr ? GetGCEquipmentComponent()->GetCurrentRangeWeapon()->GetAimFovSpeed() : 1;
    AimTimeline.TickTimeline(DeltaTime * AimFOVSpeed);
}

void APlayerCharacter::AimTimelineUpdate(const float Alpha)
{
    if (!PlayerController.IsValid())
    {
        return;
    }
    APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
    if (!IsValid(CameraManager))
    {
        return;
    }
    ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
    const float AimFOV = FMath::Lerp(CameraManager->DefaultFOV, CurrentRangeWeapon->GetAimFOV(), Alpha);
    CameraManager->SetFOV(AimFOV);
}

void APlayerCharacter::Client_SetController_Implementation()
{
    PlayerController = Cast<AGCPlayerController>(Controller);
}

void APlayerCharacter::MoveForward(float Value)
{
    if ((GetMovementComponent()->IsMovingOnGround() || GetMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 0.01f))
    {
    	FRotator YawRotator(0, GetControlRotation().Yaw, 0);
    	AddMovementInput(YawRotator.RotateVector(FVector::ForwardVector), Value);	
    }
}

void APlayerCharacter::MoveRight(float Value)
{
    if ((GetMovementComponent()->IsMovingOnGround() || GetMovementComponent()->IsFalling()) && !FMath::IsNearlyZero(Value, 0.01f))
    {
    	FRotator YawRotator(0, GetControlRotation().Yaw, 0);
    	AddMovementInput(YawRotator.RotateVector(FVector::RightVector), Value);
    }
}

void APlayerCharacter::SwimForward(float Value)
{
    if (GetMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 0.01f))
    {
    	FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0);
    	AddMovementInput(PitchYawRotator.RotateVector(FVector::ForwardVector), Value);	
    }
}

void APlayerCharacter::SwimRight(float Value)
{
    if (GetMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 0.01f))
    {
    	FRotator YawRotator(0, GetControlRotation().Yaw, 0);
    	AddMovementInput(YawRotator.RotateVector(FVector::RightVector), Value);
    }
}

void APlayerCharacter::SwimUp(float Value)
{
    if (GetMovementComponent()->IsSwimming() && !FMath::IsNearlyZero(Value, 0.01f))
    {
    	AddMovementInput(FVector::UpVector, Value);	
    }
}

void APlayerCharacter::Turn(float Value)
{
    if (!bIsAim)
    {
        AddControllerYawInput(Value);
    }
    else
    {
        ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
        AddControllerYawInput(Value * CurrentRangeWeapon->GetAimTurnModifier());
    }
}

void APlayerCharacter::LookUp(float Value)
{
    if (!bIsAim)
    {
        AddControllerPitchInput(Value);
    }
    else
    {
        ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
        AddControllerPitchInput(Value * CurrentRangeWeapon->GetAimLookUpModifier());
    }
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
    SpringArmComponent->TargetOffset = FVector(0, 0, HalfHeightAdjust);
}
void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
    SpringArmComponent->TargetOffset -= FVector(0, 0, HalfHeightAdjust);
}
bool APlayerCharacter::CanJumpInternal_Implementation() const
{
    return bIsCrouched || Super::CanJumpInternal_Implementation();
}
void APlayerCharacter::OnJumped_Implementation()
{
    if (bIsCrouched)
    {
	    UnCrouch();
    }
    Super::OnJumped_Implementation();
}

void APlayerCharacter::OnProned(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    SpringArmComponent->TargetOffset = FVector(0, 0, HalfHeightAdjust);
    const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
}

bool APlayerCharacter::AreRequiredKeysDown(EWallRunSide Side) const
{
    if (ForwardAxis < 0.1f)
    {
        return false;
    }

    if (Side == EWallRunSide::Right && RightAxis < -0.1f)
    {
        return false;
    }

    if (Side == EWallRunSide::Left && RightAxis > 0.1f)
    {
        return false;
    }

    return true;

}

void APlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController); 
    PlayerController = Cast<AGCPlayerController>(NewController);
    Client_SetController();
}

void APlayerCharacter::OnMontageTimerStarted(float Duration)
{
    Super::OnMontageTimerStarted(Duration);
}

void APlayerCharacter::OnMontageTimerElapsed()
{
    Super::OnMontageTimerElapsed();
}

void APlayerCharacter::OnStartAim_Implementation()
{
    Super::OnStartAim_Implementation();
    if (IsValid(AimTimelineCurve))
    {
        AimTimeline.Play();
    }
    else
    {
        AimTimelineUpdate(1);
    }
}

void APlayerCharacter::OnStopAim_Implementation()
{
    Super::OnStopAim_Implementation();
    if (IsValid(AimTimelineCurve))
    {
        AimTimeline.Reverse();
    }
    else
    {
        AimTimelineUpdate(0);
    }
}
