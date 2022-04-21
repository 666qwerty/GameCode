// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include "../GameCodeTypes.h"
#include "Components/GCBaseCharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Controllers/GCPlayerController.h"

AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0, 0, -86));
	FirstPersonMeshComponent->CastShadow = 0;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(false);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;
	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	RecalculateBaseEyeHeight();

	const AFPPlayerCharacter* DefaultChar = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& MeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = DefaultChar->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultChar = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& MeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = DefaultChar->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();
	if(IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
		Result.Pitch += SocketRotation.Pitch;
	}
	return Result;
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return (IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying());
}

void AFPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsFPMontagePlaying() && PlayerController.IsValid())
	{
		FRotator TargetControlRotation = PlayerController->GetControlRotation();
		TargetControlRotation.Pitch = 0;
		float BlendSpeed = 300;
		TargetControlRotation = FMath::RInterpTo(PlayerController->GetControlRotation(), TargetControlRotation, DeltaTime, BlendSpeed);
		PlayerController->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::OnLadderStarted()
{
	PlayerController->SetIgnoreCameraPitch(true);
	bUseControllerRotationYaw = false;
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	CameraManager->ViewPitchMax = LadderCameraMaxPitch;
	CameraManager->ViewPitchMin = LadderCameraMinPitch;
	CameraManager->ViewYawMax = GetGCMovementComponent()->GetForceRotation().Yaw - 90 + LadderCameraMaxYaw;
	CameraManager->ViewYawMin = GetGCMovementComponent()->GetForceRotation().Yaw - 90 + LadderCameraMinYaw;
}

void AFPPlayerCharacter::OnZiplineStarted()
{
	PlayerController->SetIgnoreCameraPitch(true);
	bUseControllerRotationYaw = false;
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	CameraManager->ViewPitchMax = ZiplineCameraMaxPitch;
	CameraManager->ViewPitchMin = ZiplineCameraMinPitch;
	CameraManager->ViewYawMax = GetGCMovementComponent()->GetForceRotation().Yaw - 90 + ZiplineCameraMaxYaw;
	CameraManager->ViewYawMin = GetGCMovementComponent()->GetForceRotation().Yaw - 90 + ZiplineCameraMinYaw;
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (PlayerController.IsValid())
	{
		if (MovementComponent->IsOnLadder())
		{
			OnLadderStarted();
		}
		else if (MovementComponent->IsOnZipline())
		{
			OnZiplineStarted();
		}
		else
		{
			PlayerController->SetIgnoreCameraPitch(false);
			bUseControllerRotationYaw = true;
			APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
			APlayerCameraManager* DefaultCameraManager = PlayerController->PlayerCameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();
			CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
			CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
			CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
			CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		}
	}
}

void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantleSettings, float StartTime)
{
	Super::OnMantle(MantleSettings, StartTime);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && MantleSettings.FPMantlingMontage)
	{
		float MontageDuration = FPAnimInstance->Montage_Play(MantleSettings.FPMantlingMontage, 1, EMontagePlayReturnType::Duration, StartTime);
	}
}

void AFPPlayerCharacter::OnMontageTimerStarted(float Duration)
{
	Super::OnMontageTimerStarted(Duration);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && FPHardLandingMontage)
	{
		float MontageDuration = FPAnimInstance->Montage_Play(FPHardLandingMontage, 1, EMontagePlayReturnType::Duration, 0);
	}
	if (PlayerController.IsValid())
	{
		PlayerController->SetIgnoreLookInput(true);
	}
}
