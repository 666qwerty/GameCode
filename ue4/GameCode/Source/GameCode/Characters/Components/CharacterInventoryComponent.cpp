// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/CharacterInventoryComponent.h"
#include "Inventory/Item/InventoryItem.h"
#include "UI/Inventory/InventoryViewWidget.h"

// Sets default values for this component's properties
UCharacterInventoryComponent::UCharacterInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InventorySlots.AddDefaulted(Capacity);
}

void UCharacterInventoryComponent::CreateViewWidget(APlayerController* Controller)
{
	if (!IsValid(Controller) || !IsValid(InventoryViewWidgetClass) || IsValid(InventoryViewWidget))
	{
		return;
	}
	InventoryViewWidget = CreateWidget<UInventoryViewWidget>(Controller, InventoryViewWidgetClass);
	InventoryViewWidget->InitializeViewWidget(InventorySlots);
}

FInventorySlot* UCharacterInventoryComponent::FindItemSlot(FName ItemID)
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {return Slot.Item.IsValid() && Slot.Item->GetDataTableID() == ItemID; });
}

FInventorySlot* UCharacterInventoryComponent::FindFreeSlot()
{
	return InventorySlots.FindByPredicate([=](const FInventorySlot& Slot) {return !Slot.Item.IsValid(); });
}


// Called every frame
void UCharacterInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCharacterInventoryComponent::OpenViewInventory(APlayerController* Controller)
{
	if (!IsValid(InventoryViewWidget))
	{
		CreateViewWidget(Controller);
	}

	if (!InventoryViewWidget->IsVisible())
	{
		InventoryViewWidget->AddToViewport();
	}
}

void UCharacterInventoryComponent::CloseViewInventory()
{
	if (InventoryViewWidget->IsVisible())
	{
		InventoryViewWidget->RemoveFromParent();
	}
}

bool UCharacterInventoryComponent::IsViewVisible() const
{
	return IsValid(InventoryViewWidget) && InventoryViewWidget->IsVisible();
}

bool UCharacterInventoryComponent::AddItem(TWeakObjectPtr<UInventoryItem> ItemToAdd, int32 Count)
{
	if (!ItemToAdd.IsValid())
	{
		return false;
	}

	FInventorySlot* FreeSlot = FindItemSlot(ItemToAdd->GetDataTableID());

	if (FreeSlot == nullptr)
	{
		FreeSlot = FindFreeSlot();
		if (FreeSlot != nullptr)
		{
			FreeSlot->Item = ItemToAdd;
			FreeSlot->Item->Amount = Count;
			ItemsInInventory++;
			FreeSlot->UpdateSlotState();
		}
		return true;
	}
	else
	{
		Count = FreeSlot->Item->Amount + Count;
		FreeSlot->Item->Amount = Count;
		FreeSlot->UpdateSlotState();
		return true;
	}
	return false;
}


bool UCharacterInventoryComponent::RemoveItem(FName ItemID)
{
	FInventorySlot* Slot = FindItemSlot(ItemID);
	if (Slot != nullptr)
	{
		InventorySlots.RemoveAll([=](const FInventorySlot& Slot) {return Slot.Item->GetDataTableID() == ItemID; });
		return true;
	}
	return false;
}

TArray<FInventorySlot> UCharacterInventoryComponent::GetAllItemsCopy() const
{
	return InventorySlots;
}

TArray<FText> UCharacterInventoryComponent::GetAllItemsNames() const
{
	TArray<FText> Result;
	for (const FInventorySlot& Slot : InventorySlots)
	{
		if (Slot.Item.IsValid())
		{
			Result.Add(Slot.Item->GetDescription().Name);
		}
	}
	return Result;
}

void FInventorySlot::BindOnInventorySlotUpdate(const FInventorySlotUpdate& Callback) const
{
	OnInventorySlotUpdate = Callback;
}

void FInventorySlot::UnBindOnInventorySlotUpdate()
{
	OnInventorySlotUpdate.Unbind();
}

void FInventorySlot::UpdateSlotState()
{
	OnInventorySlotUpdate.ExecuteIfBound();
}

void FInventorySlot::ClearSlot()
{
	Item = nullptr;
	UpdateSlotState();
}
