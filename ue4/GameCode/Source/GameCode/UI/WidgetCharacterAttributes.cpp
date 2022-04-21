// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetCharacterAttributes.h"


void UWidgetCharacterAttributes::UpdateAmount(float NewAmount, bool bIsVisible)
{
	Amount = NewAmount;
	EVisibility = bIsVisible && bCanBeVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
}