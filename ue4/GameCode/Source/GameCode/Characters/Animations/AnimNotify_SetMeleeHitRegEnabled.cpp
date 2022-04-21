// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animations/AnimNotify_SetMeleeHitRegEnabled.h"
#include "Characters/GCBaseCharacter.h"
#include "Characters/Components/CharacterEquipmentComponent.h"
#include "Equipment/Weapons/MeleeWeaponItem.h"


void UAnimNotify_SetMeleeHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	AMeleeWeaponItem* MeleeWeaponItem = CharacterOwner->GetGCEquipmentComponent()->GetCurrentMeleeWeapon();
	if (IsValid(MeleeWeaponItem))
	{
		MeleeWeaponItem->SetIsHitRegistratorsEnabled(bIsHitRegistrationEnabled);
	}
}