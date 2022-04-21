// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/Weapons/RangeWeaponItem.h"
#include "BowWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API ABowWeapon : public ARangeWeaponItem
{
	GENERATED_BODY()

public:
	virtual int32 AnimReloadAmmo() override;
	virtual void EndReload(bool bIsSuccess, bool bJumpToEnd = false) override;
	virtual void StartAim() override;
	virtual void StopAim() override;
	virtual void OnShot() override;
protected:
	virtual void BeginPlay() override;
	virtual void MakeShot() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ArrowSocketName;

private:
	void PrepareArrow();
	void HideArrow();
	bool bIsArrowEquipped;
	bool bProjectileAttached = false;
};
