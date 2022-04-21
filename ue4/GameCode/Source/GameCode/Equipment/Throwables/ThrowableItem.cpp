// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Throwables/ThrowableItem.h"
#include "GameCodeTypes.h"
#include "Characters/GCBaseCharacter.h"
#include "Actors/Projectiles/GCProjectile.h"
#include "Net/UnrealNetwork.h"


AThrowableItem::AThrowableItem()
{
	SetReplicates(true);
}

void AThrowableItem::Throw()
{
	if (!IsValid(GetCharacterOwner()))
	{
		return;
	}
	FVector ShotStart;
	FRotator PlayerViewRotation;

	if (GetCharacterOwner()->IsPlayerControlled())
	{
		APlayerController* Controller = GetCharacterOwner()->GetController<APlayerController>();
		Controller->GetPlayerViewPoint(ShotStart, PlayerViewRotation);
	}
	else
	{
		ShotStart = GetActorLocation();
		PlayerViewRotation = GetCharacterOwner()->GetBaseAimRotation();
	}
	FTransform PlayerViewTransform(PlayerViewRotation, ShotStart);
	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);
	FVector Direction = (ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector).GetSafeNormal();

	FVector SocketLocation = GetCharacterOwner()->GetMesh()->GetSocketLocation(SocketCharacterThrowable);

	FShotInfo ShotInfo(SocketLocation, Direction, CurrentProjectileIndex);
	if (!HasAuthority())
	{
		Server_Shot(ShotInfo);
	}
	ShotInternal(ShotInfo);
}

void AThrowableItem::Server_Shot_Implementation(FShotInfo ShotInfo)
{
	ShotInternal(ShotInfo);
}

void AThrowableItem::ShotInternal(FShotInfo ShotInfo)
{
	if (HasAuthority())
	{
		LastShotInfo = ShotInfo;
	}

	AGCProjectile* Projectile = ProjectilePool[ShotInfo.Index];
	if (!IsValid(Projectile))
	{
		return;
	}
	Projectile->SetActorLocation(ShotInfo.GetLocation());
	Projectile->SetActorRotation(ShotInfo.GetDirection().ToOrientationRotator()); 
	Projectile->SetActorHiddenInGame(false);
	Projectile->SetActorEnableCollision(true);
	Projectile->SetProjectileActive(true);

	Projectile->OnProjectileEnd.AddDynamic(this, &AThrowableItem::ProcessProjectileEnd);

	CurrentProjectileIndex++;
	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
	Projectile->LaunchProjectile(ShotInfo.GetDirection());
}

void AThrowableItem::OnRep_LastShotsInfo()
{
	ShotInternal(LastShotInfo);
}

void AThrowableItem::ProcessProjectileEnd(AGCProjectile* Projectile)
{
	Projectile->SetActorHiddenInGame(true);
	Projectile->SetActorEnableCollision(false);
	Projectile->SetProjectileActive(false);
	Projectile->SetActorLocation(ProjectilePoolLocation);
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->OnProjectileEnd.RemoveAll(this);
}

void AThrowableItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 
	FDoRepLifetimeParams RepParams;
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(AThrowableItem, LastShotInfo, RepParams);
	DOREPLIFETIME(AThrowableItem, ProjectilePool);
	DOREPLIFETIME(AThrowableItem, CurrentProjectileIndex);
}

// Called when the game starts
void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}
	if (!IsValid(ProjectileClass))
	{
		return;
	}
	ProjectilePool.Reserve(ProjectilePoolSize);
	for (int32 i = 0; i < ProjectilePoolSize; i++)
	{
		AGCProjectile* Projectile = GetWorld()->SpawnActor<AGCProjectile>(ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		if (!IsValid(Projectile))
		{
			return;
		}
		Projectile->SetOwner(this);
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}

}

