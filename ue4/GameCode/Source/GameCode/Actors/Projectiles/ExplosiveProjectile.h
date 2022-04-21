// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/GCProjectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
class UExplosionComponent;

UCLASS()
class GAMECODE_API AExplosiveProjectile : public AGCProjectile
{
	GENERATED_BODY()
public:
	AExplosiveProjectile();
	virtual void BeginPlay() override;
protected:
	virtual void OnProjectileLaunched() override; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UExplosionComponent *ExplosionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DetonationTime = 2;
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
private:
	virtual void OnDetonationTimerElapsed();
	AController* GetController();
	FTimerHandle DetonationTimer;
	
};
