// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Weapons/MeleeWeaponItem.h"
#include "Equipment/Weapons/Components/MeleeHitRegistration.h"
#include "Characters/GCBaseCharacter.h"

AMeleeWeaponItem::AMeleeWeaponItem()
{
	EquippedSocketName = SocketCharacterWeapon;
}

void AMeleeWeaponItem::StartAttack(EMeleeAttackType AttackType)
{
	if (!IsValid(GetCharacterOwner()))
	{
		return;
	}
	HitActors.Empty();
	CurrentAttack = Attacks.Find(AttackType);
	if (CurrentAttack && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* CharacterAnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
		if (IsValid(CharacterAnimInstance))
		{
			float Duration = CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeaponItem::OnAttackTimerElapsed, Duration, false);
		}
	}
}

void AMeleeWeaponItem::SetIsHitRegistratorsEnabled(bool bIsHitRegistratorsEnabled)
{
	HitActors.Empty();
	for (UMeleeHitRegistration* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->SetIsHitRegistrationEnabled(bIsHitRegistratorsEnabled);
	}
}

void AMeleeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistration>(HitRegistrators);
	for (UMeleeHitRegistration* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->OnMeleeHitRegistered.AddUFunction(this, FName("ProcessHit"));
	}
}

void AMeleeWeaponItem::ProcessHit(const FHitResult& HitResult, const FVector& HitDirection)
{
	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor) || CurrentAttack == nullptr || HitActors.Contains(HitActor))
	{
		return;
	}
	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = HitDirection;
	DamageEvent.DamageTypeClass = CurrentAttack->DamageTypeClass;
	AController* Controller = IsValid(GetCharacterOwner()) ? GetCharacterOwner()->GetController() : nullptr;
	HitActor->TakeDamage(CurrentAttack->DamageAmount, DamageEvent, Controller, GetOwner());
	HitActors.Add(HitActor);
}

void AMeleeWeaponItem::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
	SetIsHitRegistratorsEnabled(false);
}