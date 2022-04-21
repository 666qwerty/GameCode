// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHUDWidget.h"
#include "Characters/GCBaseCharacter.h"
#include "Characters/Components/CharacterAttributesComponent.h"
#include "UI/ReticleWidget.h"
#include "UI/AmmoWidget.h"
#include "UI/WidgetCharacterAttributes.h"
#include "Blueprint/WidgetTree.h"
#include "PlayerHUDWidget.h"
#include "UI/HighlightInteractable.h"

UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

void UPlayerHUDWidget::SetHighlightInteractableVisibility(bool bIsVisible)
{
	if (!IsValid(InteractableKey))
	{
		return;
	}
	if (bIsVisible)
	{
		InteractableKey->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUDWidget::SetHighlightInteractableActionText(FName KeyName)
{
	if (IsValid(InteractableKey))
	{
		InteractableKey->SetActionText(KeyName);
	}
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetStaminaWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(StaminaWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetOxygenWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(OxygenWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetHealthWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(HealthWidgetName);
}

bool UPlayerHUDWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(GetOwningPlayerPawn());
	if (!IsValid(Character))
	{
		return 0;
	}

	UWidgetCharacterAttributes* HealthWidget = GetHealthWidget();
	if (IsValid(HealthWidget))
	{
		HealthWidget->SetColor(FLinearColor::Red);
		Character->GetAttributesComponent()->OnHealthChangedEvent.AddUFunction(HealthWidget, FName("UpdateAmount"));
	}
	UWidgetCharacterAttributes* StaminaWidget = GetStaminaWidget();
	if (IsValid(StaminaWidget))
	{
		StaminaWidget->SetColor(FLinearColor::Yellow);
		Character->GetAttributesComponent()->OnStaminaChangedEvent.AddUFunction(StaminaWidget, FName("UpdateAmount"));
	}
	UWidgetCharacterAttributes* OxygenWidget = GetOxygenWidget();
	if (IsValid(OxygenWidget))
	{
		OxygenWidget->SetColor(FLinearColor::Blue);
		Character->GetAttributesComponent()->OnOxygenChangedEvent.AddUFunction(OxygenWidget, FName("UpdateAmount"));
	}

	return true;
}
