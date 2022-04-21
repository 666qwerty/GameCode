// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCodeTypes.h"
#include "Inventory/Item/Ammo/AmmoInventoryItem.h"
#include "CharacterInventoryComponent.generated.h"

class UInventoryItem;
class UInventoryViewWidget;

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()
public:
	DECLARE_DELEGATE(FInventorySlotUpdate)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItem> Item;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//int32 Count = 0;

	void BindOnInventorySlotUpdate(const FInventorySlotUpdate& Callback) const;
	void UnBindOnInventorySlotUpdate();
	void UpdateSlotState();
	void ClearSlot();
private:
	mutable FInventorySlotUpdate OnInventorySlotUpdate;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterInventoryComponent();

	void OpenViewInventory(APlayerController* Controller);
	void CloseViewInventory();
	bool IsViewVisible() const;

	int32 GetCapacity() const { return Capacity; }
	bool HasFreeSlot() const {return Capacity > ItemsInInventory;}

	bool AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count);
	bool RemoveItem(FName ItemID);

	TArray<FInventorySlot> GetAllItemsCopy() const;
	TArray<FText> GetAllItemsNames() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void CreateViewWidget(APlayerController* Controller);

	FInventorySlot* FindItemSlot(FName ItemID);
	FInventorySlot* FindFreeSlot();

	UPROPERTY(EditAnywhere)
	TArray<FInventorySlot> InventorySlots;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UInventoryViewWidget> InventoryViewWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Capacity = 16;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInventoryViewWidget* InventoryViewWidget;

	int32 ItemsInInventory = 0;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
