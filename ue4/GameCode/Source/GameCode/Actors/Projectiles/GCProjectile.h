// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GCProjectile.generated.h"

class AGCProjectile;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHit, AGCProjectile*, Projectile, const FHitResult&, Hit, const FVector&, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnd, AGCProjectile*, Projectile);

UCLASS()
class GAMECODE_API AGCProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGCProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHit;

	FOnEnd OnProjectileEnd;

	UFUNCTION()
	void ProjectileEnd() { OnProjectileEnd.Broadcast(this); }

	UFUNCTION(BlueprintNativeEvent)
	void SetProjectileActive(bool bIsProjectileActive);

	void SetCharge(float Charge_In) { Charge = Charge_In; }
	float GetCharge() { return Charge; }

	void SetSpeed(float Speed);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCreatePickable;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PickableName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PickableOffset = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Mesh;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnProjectileLaunched() {}
	UFUNCTION()
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	float Charge = 1;

};
