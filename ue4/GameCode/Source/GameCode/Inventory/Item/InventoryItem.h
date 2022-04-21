// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GameCodeTypes.h"
#include "InventoryItem.generated.h"

class AGCBaseCharacter;
class APickableItem;
class AEquipableItem;
class UInventoryItem;

USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APickableItem> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AEquipableItem> EquipableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInventoryItemDescription WeaponItemDescription;
};

USTRUCT(BlueprintType)
struct FAmmoTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APickableItem> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Amount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAmmoType Type = EAmmoType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInventoryItemDescription InventoryItemDescription;
};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APickableItem> PickableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventoryItem> InventoryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FInventoryItemDescription InventoryItemDescription;

};
/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(FName DataTableID_In, const FInventoryItemDescription& Description_In);
	FName GetDataTableID() const { return DataTableID; }
	const FInventoryItemDescription& GetDescription() const { return Description; }

	virtual bool IsEquipable() const { return bIsEquipable; };
	virtual bool IsConsumable() const { return bIsConsumable; };

	virtual bool Consume(AGCBaseCharacter* ConsumeTarget) PURE_VIRTUAL(UInventoryItem::Consume, return false;);
	virtual bool Spawn(const FTransform& SpawnPlace) { return false; }

	int32 Amount = 0;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DataTableID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FInventoryItemDescription Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsEquipable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsConsumable = false;
private:
	bool bIsInitialized = false;
};
