// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Item/Equipables/WeaponInventoryItem.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "Utils/GCDataTableUtils.h"

UWeaponInventoryItem::UWeaponInventoryItem()
{
	bIsConsumable = false;
}

bool UWeaponInventoryItem::Spawn(const FTransform& SpawnPlace)
{
	FWeaponTableRow* ItemData = GCDataTableUtils::FindWeaponData(DataTableID);

	if (ItemData == nullptr)
	{
		return false;
	}

	GetWorld()->SpawnActor<APickableItem>(ItemData->PickableActor, SpawnPlace);
	return true;
}