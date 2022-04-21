// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Ammo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 TotalAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Grenade;
private:
	UFUNCTION()
	void UpdateAmmoCount(int32 AmmoCount, int32 TotalAmmoCount, int32 GrenadeCount);
};
