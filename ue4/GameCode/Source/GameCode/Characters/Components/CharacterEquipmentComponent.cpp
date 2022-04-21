// Fill out your copyright notice in the Description page of Project Settings.Equipped


#include "Characters/Components/CharacterEquipmentComponent.h"
#include "Equipment/Weapons/RangeWeaponItem.h"
#include "Equipment/Weapons/MeleeWeaponItem.h"
#include "Equipment/Throwables/ThrowableItem.h"
#include "Characters/GCBaseCharacter.h"
#include "Actors/Interactive/Pickables/PickableAmmo.h"
#include "Net/UnrealNetwork.h"
#include "CharacterEquipmentComponent.h"
#include "UI/Equipment/EquipmentViewWidget.h"
#include "UI/Equipment/WeaponWheelWidget.h"

// Sets default values for this component's properties
UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

void UCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterEquipmentComponent, CurrentEquippedSlot);
	DOREPLIFETIME(UCharacterEquipmentComponent, AmmunitionArray);
	DOREPLIFETIME(UCharacterEquipmentComponent, ItemsArray);
}

void UCharacterEquipmentComponent::Server_EquipItemInSlot_Implementation(EEquipmentSlots Slot)
{
	EquipItemInSlot(Slot);
}

void UCharacterEquipmentComponent::OnRep_ItemsArray()
{
	for (AEquipableItem* Item : ItemsArray)
	{
		if (IsValid(Item))
		{
			Item->UnEquip();
		}
	}
}

EEquipableItemType UCharacterEquipmentComponent::GetEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedItem))
	{
		return CurrentEquippedItem->GetItemType();
	}
	return EEquipableItemType();
}

EAmmoType UCharacterEquipmentComponent::GetAmmoType() const
{
	if (IsValid(CurrentEquippedWeapon))
	{
		return CurrentEquippedWeapon->GetAmmoType();
	}
	return EAmmoType::None;
}

void UCharacterEquipmentComponent::Reload()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvailableAmmo = GetCurrentWeaponAmmunition();
	if (AvailableAmmo>0 && !CurrentEquippedWeapon->GetIsFull())
	{
		CurrentEquippedWeapon->StartReload();
	}
}

void UCharacterEquipmentComponent::AutoEquip()
{
	if (AutoEquipItemInSlot != EEquipmentSlots::None)
	{
		EquipItemInSlot(AutoEquipItemInSlot);
	}
}

void UCharacterEquipmentComponent::AnimReloadAmmoInCurrentWeapon(int32 NumberOfAmmo)
{
	ReloadAmmoInCurrentWeapon(CurrentEquippedWeapon->AnimReloadAmmo() * NumberOfAmmo);
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo)
{
	int32 AvailableAmmo = GetCurrentWeaponAmmunition();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentEquippedWeapon->GetAmmo();
	int32 ReloadedAmmo = FMath::Min(FMath::Min(AmmoToReload, AvailableAmmo), NumberOfAmmo);
	APickableAmmo::AddAmmo(CachedBaseCharacterOwner.Get(), AmunitionNames[GetAmmoType()], -ReloadedAmmo);
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentEquippedWeapon->GetAmmo());
	if (NumberOfAmmo < CurrentEquippedWeapon->GetMaxAmmo())
	{
		if (GetCurrentWeaponAmmunition() == 0 || CurrentEquippedWeapon->GetIsFull())
		{
			CurrentEquippedWeapon->EndReload(true, true);
		}
		else
		{
			CurrentEquippedWeapon->DisruptFullReload();
		}
	}
}

void UCharacterEquipmentComponent::OnCurrentWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon(CurrentEquippedWeapon->GetMaxAmmo());
}


void UCharacterEquipmentComponent::UnequipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnequippedSocketName());
		CurrentEquippedItem->UnEquip();
	}
	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadHandle);
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}
	PreviousEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::EndEquipItem()
{
	bIsEquipping = false;
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if (IsValid(CurrentThrowableItem))
	{
		if (CachedBaseCharacterOwner->IsLocallyControlled() || (GetOwner()->GetLocalRole() == ROLE_Authority && !CachedBaseCharacterOwner->IsPlayerControlled()))
		{
			CurrentThrowableItem->Throw();
		}
		bIsEquipping = false;
		APickableAmmo::AddAmmo(CachedBaseCharacterOwner.Get(), AmunitionNames[CurrentThrowableItem->GetAmmoType()], -1);
		if (PreviousEquippedSlot == EEquipmentSlots::None)
		{
			OnCurrentWeaponAmmoChanged(0);
		}
		if (CachedBaseCharacterOwner->IsLocallyControlled() || (GetOwner()->GetLocalRole() == ROLE_Authority && !CachedBaseCharacterOwner->IsPlayerControlled()))
		{
			EquipItemInSlot(PreviousEquippedSlot);
		}
	}
}

bool UCharacterEquipmentComponent::IsReadyToEquip(AEquipableItem* Item)
{
	CurrentThrowableItem = Cast<AThrowableItem>(Item);
	if (IsValid(CurrentThrowableItem) && AmmunitionArray[(uint32)CurrentThrowableItem->GetAmmoType()] == 0)
	{
		return false;
	}
	return true;
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquipping || !IsReadyToEquip(ItemsArray[(uint32)Slot]))
	{
		return;
	}
	UnequipCurrentItem();
	CurrentEquippedItem = ItemsArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentMeleeWeapon = Cast<AMeleeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	if(IsValid(CurrentEquippedItem))
	{
		UAnimMontage* Montage = CurrentEquippedItem->GetCharacterEquipAnimMontage();	
		if (IsValid(Montage))
		{
			bIsEquipping = true;	
			UAnimInstance* AnimInstance = (CachedBaseCharacterOwner->GetMesh()) ? CachedBaseCharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
			float const Duration = AnimInstance->Montage_Play(Montage, 1, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(MontageTimer, this, &UCharacterEquipmentComponent::EndEquipItem, Duration, false);
		}
		else
		{
			AttachCurrentItemToEquippedSocket();
		}
		CurrentEquippedItem->Equip();
	}
	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnCurrentWeaponReloadComplete"));
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}
	OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	CurrentEquippedSlot = Slot;	
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_EquipItemInSlot(Slot);
	}
}

void UCharacterEquipmentComponent::ChangeWeaponMode()
{
	if (!IsValid(CurrentEquippedWeapon))
	{
		return;
	}
	CurrentEquippedWeapon->SetAlternativeWeapon();
	OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
}

void UCharacterEquipmentComponent::EquipNextSlot()
{
	if (!CachedBaseCharacterOwner.IsValid())
	{
		return;
	}
	if (CachedBaseCharacterOwner->IsPlayerControlled())
	{
		if (IsSelectingWeapon())
		{
			WeaponWheelWidget->NextSegment();
		}
		else
		{
			APlayerController* Controller = CachedBaseCharacterOwner->GetController<APlayerController>();
			OpenWeaponWheel(Controller);
		}
		return;
	}

	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = (CurrentSlotIndex + 1) % (uint32)EEquipmentSlots::Max;

	while (CurrentSlotIndex != NextSlotIndex && (!IsValid(ItemsArray[NextSlotIndex]) || IgnoredSlotsWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex)))
	{
		NextSlotIndex = (NextSlotIndex + 1) % (uint32)EEquipmentSlots::Max;
	}
	if (CurrentSlotIndex != NextSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousSlot()
{
	if (CachedBaseCharacterOwner->IsPlayerControlled())
	{
		if (IsSelectingWeapon())
		{
			WeaponWheelWidget->PreviousSegment();
		}
		else
		{
			APlayerController* Controller = CachedBaseCharacterOwner->GetController<APlayerController>();
			OpenWeaponWheel(Controller);
		}
		return;
	}
	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = (CurrentSlotIndex - 1 + (uint32)EEquipmentSlots::Max) % (uint32)EEquipmentSlots::Max;
	while (CurrentSlotIndex != PreviousSlotIndex && (!IsValid(ItemsArray[PreviousSlotIndex]) || IgnoredSlotsWhileSwitching.Contains((EEquipmentSlots)PreviousSlotIndex)))
	{
		PreviousSlotIndex = (PreviousSlotIndex - 1 + (uint32)EEquipmentSlots::Max) % (uint32)EEquipmentSlots::Max;
	}
	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

void UCharacterEquipmentComponent::OpenWeaponWheel(APlayerController* Controller)
{
	if (!WeaponWheelWidget.IsValid())
	{
		CreateViewWidget(Controller);
	}
	if (!WeaponWheelWidget->IsVisible())
	{
		WeaponWheelWidget->AddToViewport();
	}
}

void UCharacterEquipmentComponent::ConfirmWeaponSelection()
{
	WeaponWheelWidget->ConfirmSelection();
}

bool UCharacterEquipmentComponent::IsSelectingWeapon()const 
{ 
	return WeaponWheelWidget.IsValid() && WeaponWheelWidget->IsVisible();
}
// Called when the game starts
void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterEquipmentComponent has wrong owner"));
	CachedBaseCharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
	// ...
	
}

void UCharacterEquipmentComponent::CreateLoadout()
{

	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		return;
	}
	ItemsArray.AddZeroed((uint32)EEquipmentSlots::Max);
	AmmunitionArray.AddZeroed((uint32)EAmmoType::Max);
	for (auto& ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}
		AddEquipmentItem(ItemPair.Value, (int32)ItemPair.Key);
	}
	for (auto& AmmoPair : MaxAmunitionAmount)
	{
		APickableAmmo::AddAmmo(CachedBaseCharacterOwner.Get(), AmunitionNames[AmmoPair.Key], AmmoPair.Value);

		//AmmunitionArray[(uint32)AmmoPair.Key] = AmmoPair.Value;
	}
	OnCurrentWeaponAmmoChanged(0);
}

// Called every frame
void UCharacterEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UCharacterEquipmentComponent::OnRep_CurrentEquippedSlot(EEquipmentSlots SlotOld)
{
	bIsEquipping = false;
	EquipItemInSlot(CurrentEquippedSlot);
}

void UCharacterEquipmentComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	if ((uint32)CurrentEquippedSlot == SlotIndex)
	{
		UnequipCurrentItem();
	}
	ItemsArray[SlotIndex]->Destroy();
	ItemsArray[SlotIndex] = nullptr;
}

void UCharacterEquipmentComponent::OpenViewEquipment(APlayerController* PlayerController)
{
	if (!ViewWidget.IsValid())
	{
		CreateViewWidget(PlayerController);
	}

	if (!ViewWidget->IsVisible())
	{
		ViewWidget->AddToViewport();
	}
}

void UCharacterEquipmentComponent::CloseViewEquipment()
{
	if (ViewWidget->IsVisible())
	{
		ViewWidget->RemoveFromParent();
	}
}

bool UCharacterEquipmentComponent::IsViewVisible() const
{
	return ViewWidget.IsValid() && ViewWidget->IsVisible();
}

void UCharacterEquipmentComponent::CreateViewWidget(APlayerController* PlayerController)
{
	checkf(IsValid(ViewWidgetClass), TEXT("UCharacterEquipmentComponent::CreateViewWidget view widget class is not defined"));

	if (!IsValid(PlayerController))
	{
		return;
	}

	ViewWidget = CreateWidget<UEquipmentViewWidget>(PlayerController, ViewWidgetClass);
	ViewWidget->InitializeEquipmentWidget(this);
	WeaponWheelWidget = CreateWidget<UWeaponWheelWidget>(PlayerController, WeaponWheelClass);
	WeaponWheelWidget->InitializeWheelWidget(this);
}


bool UCharacterEquipmentComponent::AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex)
{
	if (!IsValid(EquipableItemClass))
	{
		return false;
	}

	AEquipableItem* DefaultItemObject = EquipableItemClass->GetDefaultObject<AEquipableItem>();
	if (!DefaultItemObject->IsSlotCompatible((EEquipmentSlots)SlotIndex))
	{
		return false;
	}

	if (!IsValid(ItemsArray[SlotIndex]))
	{
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(EquipableItemClass);
		Item->AttachToComponent(CachedBaseCharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnequippedSocketName());
		Item->SetOwner(CachedBaseCharacterOwner.Get());
		Item->UnEquip();
		ItemsArray[SlotIndex] = Item;
	}
	else if (DefaultItemObject->IsA<ARangeWeaponItem>())
	{
		ARangeWeaponItem* RangeWeaponObject = StaticCast<ARangeWeaponItem*>(DefaultItemObject);
		int32 AmmoSlotIndex = (int32)RangeWeaponObject->GetAmmoType();
		AmmunitionArray[SlotIndex] += RangeWeaponObject->GetMaxAmmo();
		OnCurrentWeaponAmmoChanged(RangeWeaponObject->GetAmmo());
	}

	return true;
}

void UCharacterEquipmentComponent::AddAmmo(EAmmoType Type, int32 Amount)
{
	AmmunitionArray[(int32)Type] += Amount;
	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}
}