// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ReticleWidget.h"
#include "Equipment/EquipableItem.h"

void UReticleWidget::OnAimingStateChanged_Implementation(bool bIsAiming)
{
	SetupCurrentReticle();
}

void UReticleWidget::OnEquippedItemChanged_Implementation(const AEquipableItem* EquippedItem)
{
	if (CurrentEquippedItem.IsValid() && CurrentEquippedItem->GetReticleType() == EReticleType::Bow)
	{
		CurrentEquippedItem->OnChargeChanged.Remove(OnChargeChangedHandle);
	}
	CurrentEquippedItem = EquippedItem;
	
	SetupCurrentReticle();
}

void UReticleWidget::OnChargeChanged(float DamagePercent, float ReticleOffset)
{
	CurrentDamagePercent = DamagePercent;
	
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);
	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadX = FMath::Sin(RotationAngle);
	Bow->SetRenderTranslation(FVector2D(SpreadY * ReticleOffset, SpreadX * ReticleOffset));
	EChargeVisibility = DamagePercent > 0.01 ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}

void UReticleWidget::SetupCurrentReticle()
{
	CurrentReticle = CurrentEquippedItem.IsValid() ? CurrentEquippedItem->GetReticleType() : EReticleType::None;
	if (CurrentReticle == EReticleType::Bow)
	{
		OnChargeChangedHandle = CurrentEquippedItem->OnChargeChanged.AddUFunction(this, FName("OnChargeChanged"));
	}
}
