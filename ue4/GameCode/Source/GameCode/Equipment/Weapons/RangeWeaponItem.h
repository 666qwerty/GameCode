// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipableItem.h"
#include "RangeWeaponItem.generated.h"

/**
 * 
 */
class UAnimMontage;
class UCurveFloat;
DECLARE_MULTICAST_DELEGATE(FOnReloadComplete)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChanged, int32)

UCLASS(Blueprintable)
class GAMECODE_API ARangeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()

public:
	ARangeWeaponItem();
	void StartFire();
	void StopFire();
	virtual void StartAim();
	virtual void StopAim();
	void StartReload();
	void DisruptFullReload() {GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);}
	virtual void EndReload(bool bIsSuccess, bool bJumpToEnd = false);
	EAmmoType GetAmmoType() const;
	int32 GetAmmo() const;
	int32 GetMaxAmmo() const;
	EWeaponFireMode GetWeaponFireMode() const;
	void SetAmmo(int32 NewAmmo);
	bool CanShoot()const;
	float GetAimFOV() const { return AimFOV; }
	float GetAimMaxSpeed() const { return AimMaxSpeed; }
	float GetAimTurnModifier() const { return AimTurnModifier; }
	float GetAimLookUpModifier() const { return AimLookUpModifier; }
	bool IsFiring() const { return bIsFiring; }
	bool IsReloading() const { return ReloadingRate !=0; }
	bool GetIsFull() const { return GetAmmo() == GetMaxAmmo(); }
	virtual EReticleType GetReticleType() const { return bIsAim ? AimReticleType : ReticleType; }
	void SetAlternativeWeapon();
	int32 GetReloadRate() { return ReloadingRate; }
	virtual int32 AnimReloadAmmo() { return 1; }

	FTransform GetForeGripTransform() const;
	float GetAimFovSpeed()const {return AimFovSpeed;}

	FOnAmmoChanged OnAmmoChanged;
	FOnReloadComplete OnReloadComplete;

	UFUNCTION()
	virtual void OnShot();
	
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UWeaponBarrelComponent* WeaponBarrel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UWeaponBarrelComponent* AlternativeWeaponBarrel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* WeaponFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* CharacterFireMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* WeaponReloadMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* CharacterReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* WeaponAimingMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* CharacterAimingMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SpreadAngle = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimSpreadAngle = 0.25;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimMaxSpeed = 200;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimFOV = 60;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimTurnModifier = 0.25;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimLookUpModifier = 0.25;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bAlternativeWeaponEnabled = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EReticleType AimReticleType = EReticleType::Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AimFovSpeed = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultProjectileSpeed = 2000;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ChargeRate = 0.1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxCharge = 110;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* DamageChargeCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SpreadChargeCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* SpeedChargeCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* ReticleOffsetChargeCurve;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_StartAim();

	UFUNCTION(Server, Reliable)
	void Server_StartAim();

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_StopAim();

	UFUNCTION(Server, Reliable)
	void Server_StopAim();

	void AddCharge();
	void UpdateCharge();
	virtual void MakeShot();

	bool bIsAim;
	FTimerHandle ReloadTimer;
	FTimerHandle ChargeTimer;
	float CurrentCharge = 0;
	int32 ReloadingRate = 1;
private:
	class UWeaponBarrelComponent* MainWeaponBarrel;
	float GetCurrentBulletSpread() const;
	float PlayAnimMontage(UAnimMontage* montage);
	void StopAnimMontage(UAnimMontage* montage, float BlendOutTime=0);
	float GetShotTimerInterval() const;
	void OnShotTimerElapsed();
	int32 CurrenWeaponMode = 0;
	bool bIsFiring = false;
	FTimerHandle ShotTimer;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_Reload();

	UFUNCTION(Server, Reliable)
	void Server_Reload();
	UFUNCTION(Server, Reliable)
	void Server_SetAmmo(int32 Ammo);


};
