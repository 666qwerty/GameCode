// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Pickables/PickableAmmo.h"
#include "GameCodeTypes.h"
#include "Inventory/Item/Ammo/AmmoInventoryItem.h"
#include "Utils/GCDataTableUtils.h"
#include "Characters/GCBaseCharacter.h"

APickableAmmo::APickableAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);
}

void APickableAmmo::Interact(AGCBaseCharacter* Character)
{
	if (AddAmmo(Character, GetDataTableId(), 0, false))
	{
		Destroy();
	}
}

bool APickableAmmo::AddAmmo(AGCBaseCharacter* Character, FName Name, int32 Count, bool bUseCount)
{
	FAmmoTableRow* ItemData = GCDataTableUtils::FindAmmoData(Name);

	if (ItemData == nullptr)
	{
		return false;
	}

	TWeakObjectPtr<UAmmoInventoryItem> Item = NewObject<UAmmoInventoryItem>(Character);
	if (bUseCount)
	{
		Item->Amount = Count;
	}
	else
	{
		Item->Amount = ItemData->Amount;
	}
	Item->Type = ItemData->Type;
	Item->Initialize(Name, ItemData->InventoryItemDescription);

	return Character->PickupAmmo(Item);
}

FName APickableAmmo::GetActionEventName() const
{
	return ActionInteract;
}