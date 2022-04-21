// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Equipment/WeaponWheelWidget.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/Item/InventoryItem.h"
#include "Characters/Components/CharacterEquipmentComponent.h"
#include "Equipment/EquipableItem.h"
#include "Utils/GCDataTableUtils.h"
#include "Blueprint/WidgetTree.h"

void UWeaponWheelWidget::InitializeWheelWidget(UCharacterEquipmentComponent* EqipmentComponent)
{
	LinkedEquipmentComponent = EqipmentComponent;
}

void UWeaponWheelWidget::NextSegment()
{
	CurrentSegmentIndex = (CurrentSegmentIndex + 1) % EquipmentSlotSegments.Num();
	SelectSegment();
}

void UWeaponWheelWidget::PreviousSegment()
{
	CurrentSegmentIndex = (CurrentSegmentIndex - 1 + EquipmentSlotSegments.Num()) % EquipmentSlotSegments.Num(); 
	SelectSegment();
}

void UWeaponWheelWidget::ConfirmSelection()
{
	EEquipmentSlots SelectedSlot = EquipmentSlotSegments[CurrentSegmentIndex];
	LinkedEquipmentComponent->EquipItemInSlot(SelectedSlot);
	RemoveFromParent();
}

void UWeaponWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (IsValid(RadialBackground) && !IsValid(BackgroundMaterial))
	{
		BackgroundMaterial = RadialBackground->GetDynamicMaterial();
		BackgroundMaterial->SetScalarParameterValue(FName("Segments"), EquipmentSlotSegments.Num());
	}
	for (int i = 0; i < EquipmentSlotSegments.Num(); i++)
	{
		FName WidgetName = FName(FString::Printf(TEXT("ImageSegment%i"), i));
		UImage* WeaponImage = WidgetTree->FindWidget<UImage>(WidgetName);
		if (!IsValid(WeaponImage))
		{
			continue;
		}
		FWeaponTableRow* WeaponData = GetTableRowForSegment(i);
		if (WeaponData == nullptr)
		{
			WeaponImage->SetOpacity(0);
		}
		else
		{
			WeaponImage->SetOpacity(1);
			WeaponImage->SetBrushFromTexture(WeaponData->WeaponItemDescription.Icon);
		}
	}
}

void UWeaponWheelWidget::SelectSegment()
{
	BackgroundMaterial->SetScalarParameterValue(FName("Index"), CurrentSegmentIndex);
	FWeaponTableRow* WeaponData = GetTableRowForSegment(CurrentSegmentIndex);
	if (WeaponData == nullptr)
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Visible);
		WeaponNameText->SetText(WeaponData->WeaponItemDescription.Name);
	}
}

FWeaponTableRow* UWeaponWheelWidget::GetTableRowForSegment(int32 SegmentIndex) const
{
	const EEquipmentSlots& SegmentSlot = EquipmentSlotSegments[SegmentIndex];
	AEquipableItem* EquipableItem = LinkedEquipmentComponent->GetItems()[(int32)SegmentSlot];
	if (!IsValid(EquipableItem))
	{
		return nullptr;
	}
	return GCDataTableUtils::FindWeaponData(EquipableItem->GetDataTableId());
}
