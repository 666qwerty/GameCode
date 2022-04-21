// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Item/Powerups/Medkit.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "Utils/GCDataTableUtils.h"
#include "Characters/GCBaseCharacter.h"

bool UMedkit::Consume(AGCBaseCharacter* ConsumeTarget)
{
	ConsumeTarget->AddHealth(Health);
	this->ConditionalBeginDestroy();
	return true;
}

bool UMedkit::Spawn(const FTransform& SpawnPlace)
{
	FItemTableRow* ItemData = GCDataTableUtils::FindInventoryItemData(DataTableID);

	if (ItemData == nullptr)
	{
		return false;
	}

	GetWorld()->SpawnActor<APickableItem>(ItemData->PickableActorClass, SpawnPlace);
	return true;
}