// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/Weapons/Components/MeleeHitRegistration.h"
#include "DrawDebugHelpers.h"
#include "SubSystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/GCTraceUtils.h"
#include "GameCodeTypes.h"

UMeleeHitRegistration::UMeleeHitRegistration()
{
	PrimaryComponentTick.bCanEverTick = true;
	SphereRadius = 5;
	SetCollisionProfileName(CollisionProfileNoCollision);
}

void UMeleeHitRegistration::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsHitRegistrationEnabled)
	{
		ProcessHitRegistration();
	}
	PreviousComponentLocation = GetComponentLocation();

}

void UMeleeHitRegistration::ProcessHitRegistration()
{
	FVector CurrentLocation = GetComponentLocation();
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner()->GetOwner());
	QueryParams.bTraceComplex = true;

	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryMeleeWeapon) && ENABLE_DRAW_DEBUG;

	bool bHasHit = GCTraceUtils::SweepSphereSingleByChannel(
		GetWorld(),
		HitResult,
		PreviousComponentLocation,
		CurrentLocation,
		GetScaledSphereRadius(),
		ECC_Melee,
		QueryParams,
		bIsDebugEnabled,
		5
	);
	if (bHasHit)
	{
		FVector Direction = (CurrentLocation - PreviousComponentLocation).GetSafeNormal();
		OnMeleeHitRegistered.Broadcast(HitResult, Direction);
	}
}
