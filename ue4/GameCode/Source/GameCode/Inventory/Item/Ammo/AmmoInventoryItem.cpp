// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Item/Ammo/AmmoInventoryItem.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "Utils/GCDataTableUtils.h"

bool UAmmoInventoryItem::Spawn(const FTransform& SpawnPlace)
{
	return Spawn(DataTableID, SpawnPlace, GetWorld()) != nullptr;
}

APickableItem* UAmmoInventoryItem::Spawn(FName Name, const FTransform& SpawnPlace, UWorld* World)
{
	FAmmoTableRow* ItemData = GCDataTableUtils::FindAmmoData(Name);

	if (ItemData == nullptr)
	{
		return nullptr;
	}

	return World->SpawnActor<APickableItem>(ItemData->PickableActor, SpawnPlace);
}