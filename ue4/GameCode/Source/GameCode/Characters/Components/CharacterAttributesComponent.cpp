// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesComponent.h"
#include "GCBaseCharacterMovementComponent.h"
#include "../GCBaseCharacter.h"
#include "../../SubSystems/DebugSubsystem.h"
#include "../../GameCodeTypes.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

	// ...
}

void UCharacterAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAttributesComponent, Health);
}

// Called when the game starts
void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
	Stamina = MaxStamina;
	Oxygen = MaxOxygen;
	OnOxygenChangedEvent.Broadcast(Oxygen / MaxOxygen, Oxygen != MaxOxygen);
	OnStaminaChangedEvent.Broadcast(Stamina / MaxStamina, Stamina != MaxStamina);
	OnHealthChangedEvent.Broadcast(Health / MaxHealth, true);

	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UCharacterAttributesComponent has wrong owner"));
	CachedBaseCharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());
	if (GetOwner()->HasAuthority())
	{
		CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
	}
	// ...
	
}

void UCharacterAttributesComponent::OnRep_Health()
{
	OnHealthChanged();
}

void UCharacterAttributesComponent::UpdateStamina(float DeltaTime)
{
	if (!CachedBaseCharacterOwner->GetGCMovementComponent()->IsSprinting())
	{
		Tire(-StaminaRestoreVelocity * DeltaTime);
	}
	else
	{
		Tire(SprintStaminaConsumptionVelocity * DeltaTime);
	}
}

void UCharacterAttributesComponent::UpdateOxygen(float DeltaTime)
{
	float OxygenStart = Oxygen;
	if (!CachedBaseCharacterOwner->IsUnderWater())
	{
		Oxygen = FMath::Clamp(Oxygen + OxygenRestoreVelocity * DeltaTime, 0.0f, MaxOxygen);
		NoOxygenTimer = 0;
	}
	else
	{
		Oxygen = FMath::Clamp(Oxygen - SwimOxygenConsumptionVelocity * DeltaTime, 0.0f, MaxOxygen);
		if (Oxygen == 0.0f)
		{
			NoOxygenTimer += DeltaTime;
			if (NoOxygenTimer > NoOxygenInterval)
			{
				CachedBaseCharacterOwner->TakeDamage(NoOxygenDamage, FDamageEvent(), CachedBaseCharacterOwner->GetController(), CachedBaseCharacterOwner.Get());
				NoOxygenTimer = 0;
			}
		}
	}
	if (OxygenStart != Oxygen)
	{
		OnOxygenChangedEvent.Broadcast(Oxygen / MaxOxygen, Oxygen != MaxOxygen);
	}
}

void UCharacterAttributesComponent::OnHealthChanged()
{
	OnHealthChangedEvent.Broadcast(Health / MaxHealth, true);
	if (Health == 0)
	{
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

void UCharacterAttributesComponent::AddHealth(float Value)
{
	Health = FMath::Clamp(Health + Value, 0.0f, MaxHealth);
	OnHealthChanged();
}

void UCharacterAttributesComponent::AddStamina(float Value)
{
	Stamina = FMath::Clamp(Stamina + Value, 0.0f, MaxStamina);
}

void UCharacterAttributesComponent::Tire(float StaminaConsumed)
{
	float StaminaStart = Stamina;
	Stamina = FMath::Clamp(Stamina - StaminaConsumed, 0.0f, MaxStamina);
	if (Stamina == 0)
	{
		if (OutOfStaminaEvent.IsBound())
		{
			OutOfStaminaEvent.Broadcast(true);
		}
	}
	else if (Stamina == MaxStamina)
	{
		if (OutOfStaminaEvent.IsBound())
		{
			OutOfStaminaEvent.Broadcast(false);
		}
	}
	if (StaminaStart != Stamina)
	{
		OnStaminaChangedEvent.Broadcast(Stamina / MaxStamina, Stamina != MaxStamina);
	}
}

void UCharacterAttributesComponent::DebugDrawAttributes()
{
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}
	FVector TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 40.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Red, 0, true);
	TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 25.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Stamina: %.2f"), Stamina), nullptr, FColor::Yellow, 0, true);
	TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 10.0f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Oxygen: %.2f"), Oxygen), nullptr, FColor::Blue, 0, true);
#endif
}

void UCharacterAttributesComponent::OnTakeAnyDamage(AActor* Actor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
	OnHealthChanged();
}


// Called every frame
void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateStamina(DeltaTime);
	UpdateOxygen(DeltaTime);
	DebugDrawAttributes();
}

