// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Item/InventoryItem.h"
#include "Adrenaline.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UAdrenaline : public UInventoryItem
{
	GENERATED_BODY()

public:
	UAdrenaline() { bIsConsumable = true; }
	virtual bool Consume(AGCBaseCharacter* ConsumeTarget) override;
	virtual bool Spawn(const FTransform& SpawnPlace);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Adrenaline")
	float Stamina = 25.0f;
};
