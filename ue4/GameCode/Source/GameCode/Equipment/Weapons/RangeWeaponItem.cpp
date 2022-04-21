// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Weapons/RangeWeaponItem.h"
#include "Equipment/Weapons/Components/WeaponBarrelComponent.h"
#include "Characters/GCBaseCharacter.h"
#include "Characters/Components/CharacterEquipmentComponent.h"
#include "GameCodeTypes.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarrel = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("WeaponBarrel"));
	WeaponBarrel->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);
	MainWeaponBarrel = WeaponBarrel;

	AlternativeWeaponBarrel = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("AlternativeWeaponBarrel"));
	AlternativeWeaponBarrel->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	ReticleType = EReticleType::Default;

	EquippedSocketName = SocketCharacterWeapon;
}

void ARangeWeaponItem::StartFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}
	MakeShot();
	bIsFiring = true;
}

void ARangeWeaponItem::MakeShot()
{
	if (!IsValid(GetCharacterOwner()))
	{
		return;
	}
	if (!CanShoot())
	{
		StopFire();
		if ((GetAmmo() == 0) && WeaponBarrel->GetAutoReload())
		{
			GetCharacterOwner()->Reload();
		}
		return;
	}

	FVector ShotStart;
	FRotator PlayerViewRotation;
	if (GetCharacterOwner()->IsPlayerControlled())
	{
		APlayerController* Controller = GetCharacterOwner()->GetController<APlayerController>();
		if (IsValid(Controller))
		{
			Controller->GetPlayerViewPoint(ShotStart, PlayerViewRotation);
		}
	}
	else
	{
		ShotStart = WeaponBarrel->GetComponentLocation();
		PlayerViewRotation = GetCharacterOwner()->GetBaseAimRotation();
	}

	FVector Direction = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	SetAmmo(GetAmmo()-1);
	float DamagePercent = IsValid(DamageChargeCurve) ? DamageChargeCurve->GetFloatValue(CurrentCharge) : 1;
	float Speed = IsValid(SpeedChargeCurve) ? SpeedChargeCurve->GetFloatValue(CurrentCharge) : DefaultProjectileSpeed;
	WeaponBarrel->Shot(ShotStart, Direction, GetCurrentBulletSpread(), DamagePercent, Speed);
	EndReload(false);
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

float ARangeWeaponItem::GetCurrentBulletSpread() const
{ 
	if (IsValid(SpreadChargeCurve))
	{
		return FMath::DegreesToRadians(SpreadChargeCurve->GetFloatValue(CurrentCharge));
	}
	return FMath::DegreesToRadians(bIsAim ? AimSpreadAngle : SpreadAngle);
};

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring)
	{
		return;
	}
	switch (GetWeaponFireMode())
	{
	case EWeaponFireMode::Single:
		StopFire();
		break;
	case EWeaponFireMode::FullAuto:
		MakeShot();
		break;
	}
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
}

void ARangeWeaponItem::AddCharge()
{
	CurrentCharge++;
	UpdateCharge();
}

void ARangeWeaponItem::UpdateCharge()
{
	if (IsValid(DamageChargeCurve) && IsValid(ReticleOffsetChargeCurve))
	{
		float DamagePercent = DamageChargeCurve->GetFloatValue(CurrentCharge);
		float ReticleOffset = ReticleOffsetChargeCurve->GetFloatValue(CurrentCharge);
		OnChargeChanged.Broadcast(DamagePercent, ReticleOffset);
	}
	if (CurrentCharge > MaxCharge)
	{
		MakeShot();
	}
}

void ARangeWeaponItem::StartAim()
{
	if (HasAuthority())
	{
		NetMulticast_StartAim();
	}
	if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StartAim();
	}
	bIsAim = true;
	GetCharacterOwner()->OnStartAim();
}

void ARangeWeaponItem::StopAim()
{
	if (HasAuthority())
	{
		NetMulticast_StopAim();
	}
	if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_StopAim();
	}

	bIsAim = false;
	GetCharacterOwner()->OnStopAim();
}

void ARangeWeaponItem::StartReload()
{
	if (!IsValid(GetCharacterOwner()))
	{
		return;
	}
	if (HasAuthority())
	{
		NetMulticast_Reload();
	}
	if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Reload();
	}
	if (IsValid(CharacterReloadMontage))
	{
		float MontageDuration = GetCharacterOwner()->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		ReloadingRate = 1;
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this](){EndReload(true); }, MontageDuration, false);
	}
	else
	{
		EndReload(true);
	}
}

void ARangeWeaponItem::Server_Reload_Implementation()
{
	StartReload();
}

void ARangeWeaponItem::NetMulticast_Reload_Implementation()
{
	if (GetCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		StartReload();
	}
}

void ARangeWeaponItem::Server_StartAim_Implementation()
{
	StartAim();
}

void ARangeWeaponItem::NetMulticast_StartAim_Implementation()
{
	if (GetCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		StartAim();
	}
}

void ARangeWeaponItem::Server_StopAim_Implementation()
{
	StopAim();
}

void ARangeWeaponItem::NetMulticast_StopAim_Implementation()
{
	if (GetCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		StopAim();
	}
}

void ARangeWeaponItem::OnShot()
{
	GetCharacterOwner()->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);
	CurrentCharge = 0;
	UpdateCharge();
}

void ARangeWeaponItem::EndReload(bool bIsSuccess, bool bJumpToEnd)
{
	if (ReloadingRate == 0)
	{
		return;
	}
	CurrentCharge = 0;
	UpdateCharge();
	GetWorld()->GetTimerManager().ClearTimer(ChargeTimer);
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	if (bIsSuccess) 
	{
		if (bJumpToEnd)
		{
			UAnimInstance* AnimInstance = IsValid(GetCharacterOwner()) ? GetCharacterOwner()->GetMesh()->GetAnimInstance() : nullptr;
			if (IsValid(AnimInstance))
			{
				AnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, CharacterReloadMontage);
			}
			AnimInstance = WeaponMesh->GetAnimInstance();
			if (IsValid(AnimInstance))
			{
				AnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
			}
		}
		OnReloadComplete.Broadcast();
	}
	else
	{
		if (IsValid(GetCharacterOwner()))
		{
			GetCharacterOwner()->StopAnimMontage(CharacterReloadMontage);
		}
		StopAnimMontage(WeaponReloadMontage);
	}
}

EAmmoType ARangeWeaponItem::GetAmmoType() const
{
	return WeaponBarrel->GetAmmoType();
}

int32 ARangeWeaponItem::GetAmmo() const
{
	return WeaponBarrel->GetAmmo();
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return WeaponBarrel->GetMaxAmmo();
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
	WeaponBarrel->SetAmmo(NewAmmo);
	if (IsValid(GetCharacterOwner()) && GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetAmmo(NewAmmo);
	}
	OnAmmoChanged.Broadcast(NewAmmo);
}

void ARangeWeaponItem::Server_SetAmmo_Implementation(int32 NewAmmo)
{
	SetAmmo(NewAmmo);
}

bool ARangeWeaponItem::CanShoot() const
{
	return GetAmmo()>0;
}

void ARangeWeaponItem::SetAlternativeWeapon()
{
	if ((WeaponBarrel == MainWeaponBarrel) && bAlternativeWeaponEnabled)
	{
		WeaponBarrel = AlternativeWeaponBarrel;
	}
	else
	{
		WeaponBarrel = MainWeaponBarrel;
	}
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(GetMaxAmmo());
	AlternativeWeaponBarrel->SetAmmo(AlternativeWeaponBarrel->GetMaxAmmo());

	WeaponBarrel->OnShot.AddUFunction(this, FName("OnShot"));
	AlternativeWeaponBarrel->OnShot.AddUFunction(this, FName("OnShot"));
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (!IsValid(WeaponMesh->GetAnimInstance()))
		return 0;
	return WeaponAnimInstance->Montage_Play(AnimMontage);
}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (!IsValid(WeaponMesh->GetAnimInstance()))
		return;
	return WeaponAnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
}

float ARangeWeaponItem::GetShotTimerInterval() const
{
	return 60.0f / WeaponBarrel->GetRateOfFire();
}

EWeaponFireMode ARangeWeaponItem::GetWeaponFireMode() const
{
	return WeaponBarrel->GetWeaponFireMode();
}
