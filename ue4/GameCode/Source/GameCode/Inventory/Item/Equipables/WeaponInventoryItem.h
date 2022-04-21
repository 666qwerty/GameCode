// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Item/InventoryItem.h"
#include "WeaponInventoryItem.generated.h"

class AEquipableItem;
/**
 * 
 */
UCLASS()
class GAMECODE_API UWeaponInventoryItem : public UInventoryItem
{
	GENERATED_BODY()
public:
	UWeaponInventoryItem();

	void SetEquipWeaponClass(TSubclassOf<AEquipableItem>& WeaponClass) {EquipWeaponClass = WeaponClass;}
	TSubclassOf<AEquipableItem> GetEquipWeaponClass() const { return EquipWeaponClass; }
	virtual bool Spawn(const FTransform& SpawnPlace);

protected:
	TSubclassOf<AEquipableItem> EquipWeaponClass;

};
