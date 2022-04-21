// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize();
	class UWidgetCharacterAttributes* GetHealthWidget();
	class UWidgetCharacterAttributes* GetStaminaWidget();
	class UWidgetCharacterAttributes* GetOxygenWidget();
	class UReticleWidget* GetReticleWidget();
	class UAmmoWidget* GetAmmoWidget();
	void SetHighlightInteractableVisibility(bool bIsVisible);
	void SetHighlightInteractableActionText(FName KeyName);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ReticleWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AmmoWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName HealthWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName StaminaWidgetName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName OxygenWidgetName;

	UPROPERTY(meta = (BindWidget))
	class UHighlightInteractable* InteractableKey;

};
