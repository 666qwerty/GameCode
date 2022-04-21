// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameCodeTypes.h"
#include "ReticleWidget.generated.h"

/**
 * 
 */
class AEquipableItem;

UCLASS()
class GAMECODE_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquippedItemChanged(const AEquipableItem* EquippedItem);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EReticleType CurrentReticle = EReticleType::None;

	UPROPERTY(meta = (BindWidget))
	UWidget* Bow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentDamagePercent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESlateVisibility EChargeVisibility = ESlateVisibility::Hidden;
private:
	UFUNCTION()
	void OnChargeChanged(float DamagePercent, float ReticleOffset);

	TWeakObjectPtr<const AEquipableItem> CurrentEquippedItem;
	FDelegateHandle OnChargeChangedHandle;

	void SetupCurrentReticle();
};
