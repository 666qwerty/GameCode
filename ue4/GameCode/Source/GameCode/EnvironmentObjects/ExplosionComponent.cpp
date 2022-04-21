// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvironmentObjects/ExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

// Sets default values for this component's properties
UExplosionComponent::UExplosionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UExplosionComponent::Explode(AController* Controller)
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		UGameplayStatics::ApplyRadialDamageWithFalloff(
			GetWorld(),
			MaxDamage, MinDamage,
			GetComponentLocation(),
			InnerRadius, OuterRadius,
			DamageFaloff,
			DamageTypeClass,
			IgnoredActors,
			GetOwner(),
			Controller,
			ECC_Visibility
		);
	}

	if (IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation());
	}
	OnExplosion.Broadcast();

}

// Called when the game starts
void UExplosionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UExplosionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

