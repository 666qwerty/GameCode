// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Equipment/EquipableItem.h"
#include "GameCodeTypes.h"
#include "WeaponBarrelComponent.generated.h"

class UNiagaraSystem;
class AGCProjectile;

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UMaterialInterface *DecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector DecalSize = FVector(5, 5, 5);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DecalLifeTime = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DecalFadeOutTime = 5;

};


UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
	HitScan = 0,
	Projectile = 1
};

DECLARE_MULTICAST_DELEGATE(FOnShot)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UWeaponBarrelComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponBarrelComponent();
	void Shot(FVector ShotStart, FVector Direction, float SpreadAngle, float DamagePercent, float Speed);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	EAmmoType GetAmmoType() const { return AmmoType; }
	int32 GetAmmo() const { return Ammo; }
	int32 GetMaxAmmo() const { return MaxAmmo; }
	bool GetAutoReload() const { return bAutoReload; }
	void SetAmmo(int32 NewAmmo) { Ammo = NewAmmo; }
	int32 GetRateOfFire() const { return RateOfFire; }
	EWeaponFireMode GetWeaponFireMode() const { return WeaponFireMode; }
	FOnShot OnShot;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	int32 BulletsPerShot = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	EHitRegistrationType HitRegistrationType = EHitRegistrationType::HitScan;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes", meta = (EditCondition = "HitRegistrationType == EHitRegistrationType::Projectile"))
	TSubclassOf<class AGCProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	int32 ProjectilePoolSize = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	float FiringRange = 3000;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	float DamageAmount = 20;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	TSubclassOf<class UDamageType> DamageTypeClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	UCurveFloat* FalloffDiagram;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	UNiagaraSystem* MuzzleFlashFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	UNiagaraSystem* TraceFX;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BarrelAttributes")
	FDecalInfo DefaultDecalInfo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxAmmo = 30;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAmmoType AmmoType = EAmmoType::None;
	//rpm
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RateOfFire = 600;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAutoReload = true;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AGCProjectile* GetZeroProjectile() { return ProjectilePool[0]; }

private:
	void ShotInternal(const TArray<FShotInfo>& ShotsInfo);

	UFUNCTION(Server, Reliable)
	void Server_Shot(const TArray<FShotInfo>& ShotsInfo);

	UPROPERTY(ReplicatedUsing = OnRep_LastShotsInfo)
	TArray<FShotInfo> LastShotsInfo;

	UFUNCTION()
	void OnRep_LastShotsInfo();

	UPROPERTY(Replicated)
	TArray<AGCProjectile*> ProjectilePool;
	
	UPROPERTY(Replicated)
	uint32 CurrentProjectileIndex = 1;

	void LaunchProjectile(FVector ShotStart, FVector ShotDirection, float DamagePercent, float Speed);
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation);
	bool HitScan(FVector ShotStart, OUT FVector& ShotEnd, FVector ShotDirection);
	UFUNCTION()
	void ProcessHit(const FHitResult& Hit, const FVector& Direction, float Charge);
	UFUNCTION()
	void ProcessProjectileHit(AGCProjectile* Projectile, const FHitResult& Hit, const FVector& Direction);
	APawn* GetOwningPawn() const;
	AController* GetController() const;

	UPROPERTY(Replicated)
	int32 Ammo;
};
