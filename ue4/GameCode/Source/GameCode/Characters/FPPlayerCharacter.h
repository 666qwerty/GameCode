// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	AFPPlayerCharacter(const FObjectInitializer& FObjectInitializer);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;

	virtual void Tick(float DeltaTime);

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void OnLadderStarted();
	virtual void OnZiplineStarted();
protected:

	virtual void OnMantle(const FMantlingSettings& MantleSettings, float StartTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheracter | FP")
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cheracter | FP")
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Ladder")
	float LadderCameraMinPitch = -60;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Ladder")
	float LadderCameraMaxPitch = 80;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Ladder")
	float LadderCameraMinYaw = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Ladder")
	float LadderCameraMaxYaw = 175;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Zipline")
	float ZiplineCameraMinPitch = -89;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Zipline")
	float ZiplineCameraMaxPitch = 89;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Zipline")
	float ZiplineCameraMinYaw = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cheracter | FP | Camera | Zipline")
	float ZiplineCameraMaxYaw = 180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HardLanding Parameters")
	UAnimMontage* FPHardLandingMontage;

	virtual void OnMontageTimerStarted(float Duration) override;
private:
	bool IsFPMontagePlaying() const;
};
