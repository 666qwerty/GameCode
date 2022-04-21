// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameCodeTypes.h"
#include "Turret.generated.h"

class UWeaponBarrelComponent;

UENUM()
enum class ETurretState :uint8
{
	Searching = 0,
	Firing = 1
};

UCLASS()
class GAMECODE_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATurret();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PossessedBy(AController* NewController) override;

	void OnCurrentTargetSet();

	virtual FVector GetPawnViewLocation() const override;
	virtual FRotator GetViewRotation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	AActor* CurrentTarget = nullptr;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	bool bHasSeenTarget = false;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* TurretBaseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* TurretBarrelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWeaponBarrelComponent* WeaponBarrel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseFiringInterpSpeed = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseSearchingRotationRate = 60;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BarrelPitchRotationRate = 60;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxBarrelPitchAngle = 60;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RateOfFire = 300;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BulletSpreadAngle = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireDelayTime = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	ETeams Team = ETeams::Enemy;

private:
	void SearchingMovement(float DeltaTime);

	void FiringMovement(float DeltaTime);

	void MakeShot();

	void SetCurrentTurretState(ETurretState NewState);
	ETurretState CurrentTurretState = ETurretState::Searching;
	UFUNCTION()
	void OnRep_CurrentTarget();


	float GetFireInterval() const { return 60 / RateOfFire; }

	FTimerHandle ShotTimer;
};
