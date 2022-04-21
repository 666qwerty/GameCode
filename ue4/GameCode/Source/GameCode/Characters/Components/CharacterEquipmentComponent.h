// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameCodeTypes.h"
#include "CharacterEquipmentComponent.generated.h"

class AMeleeWeaponItem;
class ARangeWeaponItem;
class AThrowableItem;
class AEquipableItem;
class UEquipmentViewWidget;
class UWeaponWheelWidget;
typedef TArray<class AEquipableItem*, TInlineAllocator<(int32)EEquipmentSlots::Max>> TItemsArray;
typedef TArray<int32, TInlineAllocator<(int32)EAmmoType::Max>> TAmmunitionArray;
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnCurrentWeaponAmmoChanged, int32, int32, int32)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterEquipmentComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	EEquipableItemType GetEquippedItemType() const;
	EAmmoType GetAmmoType() const;
	ARangeWeaponItem* GetCurrentRangeWeapon() const {
		return CurrentEquippedWeapon;
	}
	AMeleeWeaponItem* GetCurrentMeleeWeapon() const {
		return CurrentMeleeWeapon;
	}
	void Reload();
	void EquipItemInSlot(EEquipmentSlots Slot);
	void EquipNextSlot();
	void UnequipCurrentItem();
	void EquipPreviousSlot();
	void AttachCurrentItemToEquippedSocket();
	void LaunchCurrentThrowableItem();
	bool GetIsEquipping() { return bIsEquipping; };
	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;
	FOnEquippedItemChanged OnEquippedItemChanged;
	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo);
	void AnimReloadAmmoInCurrentWeapon(int32 NumberOfAmmo);
	void ChangeWeaponMode();
	bool AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex);
	void RemoveItemFromSlot(int32 SlotIndex);
	void OpenViewEquipment(APlayerController* Controller);
	void CloseViewEquipment();
	bool IsViewVisible()const;
	void OpenWeaponWheel(APlayerController* Controller);
	bool IsSelectingWeapon()const;
	void ConfirmWeaponSelection();
	void AddAmmo(EAmmoType Type, int32 Amount); 

	const TArray<AEquipableItem*>& GetItems() { return ItemsArray; }

	void CreateLoadout();
	void CreateViewWidget(APlayerController* Controller);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeaponItem> SideArmClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EAmmoType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EAmmoType, FName> AmunitionNames;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSet<EEquipmentSlots> IgnoredSlotsWhileSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEquipmentSlots AutoEquipItemInSlot = EEquipmentSlots::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UEquipmentViewWidget> ViewWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UWeaponWheelWidget> WeaponWheelClass;
private:
	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlots Slot);

	UPROPERTY(Replicated)
	TArray<int32> AmmunitionArray;
	UPROPERTY(ReplicatedUsing = OnRep_ItemsArray)
	TArray<AEquipableItem*> ItemsArray;

	UFUNCTION()
	void OnRep_ItemsArray();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo) { OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetCurrentWeaponAmmunition(), GetCurrentGrenadeAmmunition()); }
	UFUNCTION()
	void OnCurrentWeaponReloadComplete();
	int32 GetCurrentWeaponAmmunition() { return AmmunitionArray[(uint32)GetAmmoType()]; }
	int32 GetCurrentGrenadeAmmunition() { return AmmunitionArray[(uint32)EAmmoType::FragGrenades]; }
	bool IsReadyToEquip(AEquipableItem* Item);
	void AutoEquip();
	void EndEquipItem();

	ARangeWeaponItem* CurrentEquippedWeapon;
	AMeleeWeaponItem* CurrentMeleeWeapon;
	AThrowableItem* CurrentThrowableItem;
	AEquipableItem* CurrentEquippedItem;
	AEquipableItem* PreviousEquippedItem;
	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadHandle;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentEquippedSlot)
	EEquipmentSlots CurrentEquippedSlot;
	UFUNCTION()
	void OnRep_CurrentEquippedSlot(EEquipmentSlots SlotOld);
	EEquipmentSlots PreviousEquippedSlot;
	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacterOwner;
	bool bIsEquipping = false;
	FTimerHandle MontageTimer;

	TWeakObjectPtr < UEquipmentViewWidget> ViewWidget;
	TWeakObjectPtr<UWeaponWheelWidget> WeaponWheelWidget;
};
