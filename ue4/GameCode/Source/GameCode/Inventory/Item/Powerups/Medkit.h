// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Item/InventoryItem.h"
#include "Medkit.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UMedkit : public UInventoryItem
{
	GENERATED_BODY()
public:
	UMedkit() { bIsConsumable = true; }
	virtual bool Consume(AGCBaseCharacter* ConsumeTarget) override;
	virtual bool Spawn(const FTransform& SpawnPlace);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Medkit")
	float Health = 25.0f;

};
