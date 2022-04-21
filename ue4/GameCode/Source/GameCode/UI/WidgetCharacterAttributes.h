// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetCharacterAttributes.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UWidgetCharacterAttributes : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetColor(FLinearColor NewColor) { Color = NewColor; }
	UFUNCTION()
	void UpdateAmount(float NewAmount, bool bIsVisible);
	bool bCanBeVisible = true;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Amount=0.5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESlateVisibility EVisibility = ESlateVisibility::Visible;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Color;
private:
};
