// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameCodeTypes.h"
#include "WeaponWheelWidget.generated.h"

struct FWeaponTableRow;
class UCharacterEquipmentComponent;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class GAMECODE_API UWeaponWheelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWheelWidget(UCharacterEquipmentComponent* EqipmentComponent);
	void NextSegment();
	void PreviousSegment();
	void ConfirmSelection();
	int32 CurrentSegmentIndex;

protected:
	virtual void NativeConstruct() override;
	void SelectSegment();

	UPROPERTY(meta = (BindWidget))
	UImage* RadialBackground;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponNameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EEquipmentSlots> EquipmentSlotSegments;
private:
	FWeaponTableRow* GetTableRowForSegment(int32 SegmentIndex) const;

	UMaterialInstanceDynamic* BackgroundMaterial;
	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent;
};
