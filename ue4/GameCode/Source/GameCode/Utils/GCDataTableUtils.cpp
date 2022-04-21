// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GCDataTableUtils.h"


FWeaponTableRow* GCDataTableUtils::FindWeaponData(FName WeaponID)
{
	static const FString ContextString(TEXT("Find Weapon Data"));
	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/GameCode/Data/DataTables/DT_WeaponList.DT_WeaponList"));
	if (WeaponDataTable == nullptr)
	{
		return nullptr;
	}
	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, ContextString);
}

FItemTableRow* GCDataTableUtils::FindInventoryItemData(FName ItemID)
{
	static const FString ContextString(TEXT("Find Item Data"));
	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/GameCode/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));
	if (InventoryItemDataTable == nullptr)
	{
		return nullptr;
	}
	return InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, ContextString);
}

FAmmoTableRow* GCDataTableUtils::FindAmmoData(FName ItemID)
{
	static const FString ContextString(TEXT("Find Ammo Data"));
	UDataTable* AmmoDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/GameCode/Data/DataTables/DT_AmmoList.DT_AmmoList"));
	if (AmmoDataTable == nullptr)
	{
		return nullptr;
	}
	return AmmoDataTable->FindRow<FAmmoTableRow>(ItemID, ContextString);
}
