// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "GameCodeTypes.h"
#include "Components/BoxComponent.h"
#include "Characters/GCBaseCharacter.h"
#include "Net/UnrealNetwork.h"

APlatformTrigger::APlatformTrigger()
{
	bReplicates = true;
	NetUpdateFrequency = 2;
	MinNetUpdateFrequency = 2;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	SetRootComponent(TriggerBox);
}

void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapEnd);
}

void APlatformTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlatformTrigger, bIsActivated);
}

void APlatformTrigger::SetIsActivated(bool bIsActivated_In)
{
	bIsActivated = bIsActivated_In;
	if (OnTriggerActivated.IsBound())
	{
		OnTriggerActivated.Broadcast(bIsActivated);
	}
}

void APlatformTrigger::OnRep_IsActivated(bool bIsActivated_Old)
{
	SetIsActivated(bIsActivated);
}

void APlatformTrigger::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGCBaseCharacter* OtherPawn = Cast<AGCBaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (OtherPawn->IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		OverlappedPawns.AddUnique(OtherPawn);

		if (!bIsActivated && OverlappedPawns.Num() > 0)
		{
			bIsActivated = true;
			SetIsActivated(true);
		}

	}
}

void APlatformTrigger::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AGCBaseCharacter* OtherPawn = Cast<AGCBaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (OtherPawn->IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		OverlappedPawns.RemoveSingleSwap(OtherPawn);

		if (bIsActivated && OverlappedPawns.Num() == 0)
		{
			bIsActivated = false;
			SetIsActivated(false);
		}
	}
}
