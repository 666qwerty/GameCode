// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Item/InventoryItem.h"
#include "AmmoInventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UAmmoInventoryItem : public UInventoryItem
{
	GENERATED_BODY()
public:
	virtual bool Spawn(const FTransform& SpawnPlace);
	static class APickableItem* Spawn(FName Name, const FTransform& SpawnPlace, UWorld* World);
	UPROPERTY(EditDefaultsOnly)
	EAmmoType Type = EAmmoType::None;
};
