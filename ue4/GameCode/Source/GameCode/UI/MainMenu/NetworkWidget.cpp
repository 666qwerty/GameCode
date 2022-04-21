// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/NetworkWidget.h"

FText UNetworkWidget::GetNetworkType() const
{
	return bIsLAN ? FText::FromString(TEXT("LAN")) : FText::FromString(TEXT("Internet"));
}

void UNetworkWidget::ToggleNetworkType()
{
	bIsLAN = !bIsLAN;
}

void UNetworkWidget::CloseWidget()
{
	OnNetworkWidgetClosed.Broadcast();
	SetVisibility(ESlateVisibility::Hidden);
}
