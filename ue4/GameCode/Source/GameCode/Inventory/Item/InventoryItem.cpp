// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Item/InventoryItem.h"

void UInventoryItem::Initialize(FName DataTableID_In, const FInventoryItemDescription& Description_In)
{
	DataTableID = DataTableID_In;
	Description = Description_In;
	bIsInitialized = true;
}