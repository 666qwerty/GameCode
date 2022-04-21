// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Weapons/BowWeapon.h"
#include "Equipment/Weapons/Components/WeaponBarrelComponent.h"
#include "Characters/GCBaseCharacter.h"
#include "Characters/Components/CharacterEquipmentComponent.h"
#include "Actors/Projectiles/GCProjectile.h"
#include "GameCodeTypes.h"


void ABowWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(0);
}

int32 ABowWeapon::AnimReloadAmmo()
{
	if (GetReloadRate() > 0)
	{
		if (GetCharacterOwner()->GetLocalRole() != ROLE_SimulatedProxy)
		{
			PrepareArrow();
		}
		bIsAim = true;
		GetCharacterOwner()->OnStartAim();
		return 1;
	}
	else
	{
		if (GetCharacterOwner()->GetLocalRole() != ROLE_SimulatedProxy)
		{
			HideArrow();
		}
		bIsAim = false;
		GetCharacterOwner()->OnStopAim();
		return -1;
	}
}

void ABowWeapon::PrepareArrow()
{
	AGCProjectile* Projectile = WeaponBarrel->GetZeroProjectile();
	Projectile->SetActorHiddenInGame(false);
	Projectile->SetActorEnableCollision(true);
	if (!bProjectileAttached)
	{
		Projectile->SetActorLocation(FVector(0, 0, 0));
		Projectile->SetActorRotation(FRotator::ZeroRotator);
		Projectile->AttachToComponent(GetCharacterOwner()->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, ArrowSocketName);
	}
	bProjectileAttached = true;
	bIsArrowEquipped = true;
}

void ABowWeapon::HideArrow()
{
	AGCProjectile* Projectile = WeaponBarrel->GetZeroProjectile();
	Projectile->SetActorHiddenInGame(true);
	Projectile->SetActorEnableCollision(false);
	bIsArrowEquipped = false;
}

void ABowWeapon::EndReload(bool bIsSuccess, bool bJumpToEnd)
{
	UAnimInstance* AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(CharacterAimingMontage);
	if (MontageInstance != nullptr)
	{
		MontageInstance->Stop(FAlphaBlend(0), true);
	}
	AnimInstance = WeaponMesh->GetAnimInstance();
	MontageInstance = AnimInstance->GetActiveInstanceForMontage(CharacterAimingMontage);
	if (MontageInstance != nullptr)
	{
		MontageInstance->Stop(FAlphaBlend(0), true);
	}
	if (bIsArrowEquipped)
	{
		HideArrow();
		GetCharacterOwner()->GetGCEquipmentComponent()->AddAmmo(GetAmmoType(), GetAmmo());
	}
	bIsAim = false;
	GetCharacterOwner()->SetIsAim(false);
	Super::EndReload(bIsSuccess, bJumpToEnd);
}


void ABowWeapon::StartAim()
{
	if (HasAuthority())
	{
		NetMulticast_StartAim();
	}
	if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StartAim();
	}

	float Position = 0;
	UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance();
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(WeaponAimingMontage);
	if (MontageInstance != nullptr)
	{
		Position = MontageInstance->GetPosition();
	}

	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Play(WeaponAimingMontage, 1, EMontagePlayReturnType::Duration, Position);
	}
	AnimInstance = IsValid(GetCharacterOwner()) ? GetCharacterOwner()->GetMesh()->GetAnimInstance() : nullptr;
	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Play(CharacterAimingMontage, 1, EMontagePlayReturnType::Duration, Position);
	}
	GetWorld()->GetTimerManager().SetTimer(ChargeTimer, this, &ABowWeapon::AddCharge, ChargeRate, true);
	ReloadingRate = 1;
}

void ABowWeapon::StopAim()
{
	if (HasAuthority())
	{
		NetMulticast_StopAim();
	}
	if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StopAim();
	}

	float Position = 0;
	UAnimInstance* AnimInstance = WeaponMesh->GetAnimInstance();
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(WeaponAimingMontage);
	if (MontageInstance != nullptr)
	{
		Position = MontageInstance->GetPosition();
	}
	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Play(WeaponAimingMontage, -1, EMontagePlayReturnType::Duration, Position);
	}
	AnimInstance = IsValid(GetCharacterOwner()) ? GetCharacterOwner()->GetMesh()->GetAnimInstance() : nullptr;
	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Play(CharacterAimingMontage, -1, EMontagePlayReturnType::Duration, Position);
	}
	ReloadingRate = -1;
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);
	CurrentCharge = 0;
	UpdateCharge();
}

void ABowWeapon::OnShot()
{
	Super::OnShot();
	GetCharacterOwner()->SetIsAim(false);
}


void ABowWeapon::MakeShot()
{
	Super::MakeShot();
	if (ReloadingRate > 0)
	{
		StartAim();
	}
	else
	{
		ReloadingRate = 0;
	}
}