// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Animations/AnimNotify_ReloadAmmo.h"
#include "Characters/GCBaseCharacter.h"
#include "Characters/Components/CharacterEquipmentComponent.h"


void UAnimNotify_ReloadAmmo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	CharacterOwner->GetGCEquipmentComponent()->AnimReloadAmmoInCurrentWeapon(NumberOfAmmo);
}