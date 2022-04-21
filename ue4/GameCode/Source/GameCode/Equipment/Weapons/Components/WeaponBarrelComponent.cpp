// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Weapons/Components/WeaponBarrelComponent.h"
#include "Actors/Projectiles/GCProjectile.h"
#include "GameCodeTypes.h"
#include "DrawDebugHelpers.h"
#include "SubSystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Perception/AISense_Damage.h"
#include "Net/UnrealNetwork.h"
#include "Characters/GCBaseCharacter.h"

// Sets default values for this component's properties
UWeaponBarrelComponent::UWeaponBarrelComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// ...
}

APawn* UWeaponBarrelComponent::GetOwningPawn() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!IsValid(Pawn))
	{
		Pawn = Cast<APawn>(GetOwner()->GetOwner());
	}
	return Pawn;
}

AController* UWeaponBarrelComponent::GetController() const
{
	APawn* Pawn = GetOwningPawn();
	if (!IsValid(Pawn))
	{
		return Pawn ->GetController();
	}
	return nullptr;
}

void UWeaponBarrelComponent::ShotInternal(const TArray<FShotInfo>& ShotsInfo)
{
	if (GetOwner()->HasAuthority())
	{
		LastShotsInfo = ShotsInfo;
	}
	FVector MuzzleStart = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleStart, GetComponentRotation());
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon) && ENABLE_DRAW_DEBUG;
	for (const FShotInfo& ShotInfo: ShotsInfo)
	{
		FVector ShotStart = ShotInfo.GetLocation();
		FVector Direction = ShotInfo.GetDirection();
		FVector ShotEnd = ShotStart + FiringRange * Direction;
		switch (HitRegistrationType)
		{
		case EHitRegistrationType::HitScan:
		{
			if (HitScan(ShotStart, ShotEnd, Direction) && bIsDebugEnabled)
			{
				DrawDebugSphere(GetWorld(), ShotEnd, 10, 24, FColor::Red, false, 1);
			}
			break;
		}
		case EHitRegistrationType::Projectile:
		{
			LaunchProjectile(ShotStart, Direction, ShotInfo.DamagePercent, ShotInfo.Speed);
			break;
		}
		}
		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleStart, GetComponentRotation());
		if (IsValid(TraceFXComponent)) {
			TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
		}
		if (bIsDebugEnabled)
		{
			DrawDebugLine(GetWorld(), MuzzleStart, ShotEnd, FColor::Red, false, 1, 0, 3);
		}
	}
	OnShot.Broadcast();
}

void UWeaponBarrelComponent::Server_Shot_Implementation(const TArray<FShotInfo>& ShotsInfo)
{
	ShotInternal(ShotsInfo);
}

void UWeaponBarrelComponent::OnRep_LastShotsInfo()
{
	ShotInternal(LastShotsInfo);
}

void UWeaponBarrelComponent::LaunchProjectile(FVector ShotStart, FVector ShotDirection, float DamagePercent, float Speed)
{
	AGCProjectile* Projectile;
	Projectile = ProjectilePool[CurrentProjectileIndex];
	if (!IsValid(Projectile))
	{
		return;
	}

	Projectile->SetActorLocation(ShotStart);
	Projectile->SetActorRotation(ShotDirection.ToOrientationRotator());
	CurrentProjectileIndex++;
	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 1;
	}
	Projectile->SetCharge(DamagePercent);
	Projectile->SetSpeed(Speed);

	Projectile->SetActorHiddenInGame(false);
	Projectile->SetActorEnableCollision(true);
	Projectile->SetProjectileActive(true);
	Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarrelComponent::ProcessProjectileHit);
	Projectile->LaunchProjectile(ShotDirection);
}

void UWeaponBarrelComponent::ProcessProjectileHit(AGCProjectile* Projectile, const FHitResult& Hit, const FVector& Direction)
{
	Projectile->SetActorHiddenInGame(true);
	Projectile->SetActorEnableCollision(false);
	Projectile->SetProjectileActive(false);
	Projectile->OnProjectileHit.RemoveAll(this);
	ProcessHit(Hit, Direction, Projectile->GetCharge());
}

bool UWeaponBarrelComponent::HitScan(FVector ShotStart, OUT FVector& ShotEnd, FVector Direction)
{
	FHitResult ShotResult;
	if (GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet))
	{
		ShotEnd = ShotResult.ImpactPoint;
		ProcessHit(ShotResult, Direction, 1);
		return true;
	}
	return false;
}

void UWeaponBarrelComponent::ProcessHit(const FHitResult& Hit, const FVector& Direction, float Charge)
{
	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, Hit.ImpactPoint, Hit.ImpactNormal.ToOrientationRotator());
	if (IsValid(DecalComponent))
	{
		DecalComponent->SetFadeScreenSize(0.0001);
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
	}
	AActor* HitActor = Hit.GetActor();
	if (GetOwner()->HasAuthority() && IsValid(HitActor))
	{
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = Hit;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;
		float RealDamageAmount = FalloffDiagram->GetFloatValue((Hit.ImpactPoint - GetComponentLocation()).Size() / FiringRange) * DamageAmount * Charge;
		HitActor->TakeDamage(RealDamageAmount, DamageEvent, GetController(), GetOwner());
		UAISense_Damage::ReportDamageEvent(GetWorld(), HitActor, GetOwningPawn(), RealDamageAmount, GetOwningPawn()->GetActorLocation(), Hit.ImpactPoint);
	}
}

void UWeaponBarrelComponent::Shot(FVector ShotStart, FVector GeneralDirection, float SpreadAngle, float DamagePercent, float Speed)
{
	TArray<FShotInfo> ShotsInfo;
	for (int i=0; i<BulletsPerShot; i++)
	{
		FVector Direction = (GeneralDirection + GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), Direction.ToOrientationRotator())).GetSafeNormal();
		ShotsInfo.Emplace(ShotStart, Direction, 0, DamagePercent, Speed);
	}
	Server_Shot(ShotsInfo);
	if (!GetOwner()->HasAuthority())
	{
		ShotInternal(ShotsInfo);
	}
}

void UWeaponBarrelComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(UWeaponBarrelComponent, LastShotsInfo, RepParams);
	DOREPLIFETIME(UWeaponBarrelComponent, ProjectilePool);
	DOREPLIFETIME(UWeaponBarrelComponent, CurrentProjectileIndex);
	DOREPLIFETIME(UWeaponBarrelComponent, Ammo);
}

FVector UWeaponBarrelComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation)
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);
	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);
	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ
		+ ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
	return Result;
}

// Called when the game starts
void UWeaponBarrelComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() < ROLE_Authority)
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
		AGCProjectile* Projectile = GetWorld()->SpawnActor<AGCProjectile>(ProjectileClass, Hell, FRotator::ZeroRotator);
		if (!IsValid(Projectile))
		{
			return;
		}
		Projectile->SetOwner(GetOwningPawn());
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}
	
}


// Called every frame
void UWeaponBarrelComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

