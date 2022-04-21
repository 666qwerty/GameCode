// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Item/Powerups/Adrenaline.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "Utils/GCDataTableUtils.h"
#include "Characters/GCBaseCharacter.h"

bool UAdrenaline::Consume(AGCBaseCharacter* ConsumeTarget)
{
	ConsumeTarget->AddStamina(Stamina);
	this->ConditionalBeginDestroy();
	return true;
}

bool UAdrenaline::Spawn(const FTransform& SpawnPlace)
{
	FItemTableRow* ItemData = GCDataTableUtils::FindInventoryItemData(DataTableID);

	if (ItemData == nullptr)
	{
		return false;
	}

	GetWorld()->SpawnActor<APickableItem>(ItemData->PickableActorClass, SpawnPlace);
	return true;
}
