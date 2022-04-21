// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipableItem.h"
#include "GameCodeTypes.h"
#include "ThrowableItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API AThrowableItem : public AEquipableItem
{
	GENERATED_BODY()
public:
	AThrowableItem();
	void Throw();
	EAmmoType GetAmmoType() const { return AmmoType; }
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void ProcessProjectileEnd(AGCProjectile* Projectile);
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class AGCProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float ThrowAngle = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAmmoType AmmoType = EAmmoType::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ProjectilePoolSize = 5;

	UPROPERTY(Replicated)
	TArray<AGCProjectile*> ProjectilePool;

	UPROPERTY(Replicated)
	uint32 CurrentProjectileIndex=0;

	void ShotInternal(FShotInfo ShotInfo);

	UFUNCTION(Server, Reliable)
	void Server_Shot(FShotInfo ShotInfo);

	UPROPERTY(ReplicatedUsing = OnRep_LastShotsInfo)
	FShotInfo LastShotInfo;

	UFUNCTION()
	void OnRep_LastShotsInfo();

	const FVector ProjectilePoolLocation = FVector(0, 0, -100);
	
};
