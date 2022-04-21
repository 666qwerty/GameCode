// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform2.h"
#include "Components/StaticMeshComponent.h"
#include "Actors/Environment/PlatformTrigger.h"

ABasePlatform2::ABasePlatform2()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 2;
	MinNetUpdateFrequency = 2;
	SetReplicateMovement(true);

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	SetRootComponent(PlatformMesh);
}

void ABasePlatform2::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(MovementCurve))
	{
		FOnTimelineFloatStatic OnTimelineUpdate;
		OnTimelineUpdate.BindUFunction(this, FName("TickPlatformTimeline"));
		PlatformTimeline.AddInterpFloat(MovementCurve, OnTimelineUpdate);

		FOnTimelineEventStatic OnTimelineFinished;
		OnTimelineFinished.BindUFunction(this, FName("OnPlatfromEndReached"));
		PlatformTimeline.SetTimelineFinishedFunc(OnTimelineFinished);
	}

	if (PlatformBehavior == EPlatformBehavior2::Loop)
	{
		MovePlatform();
	}

	StartLocation = GetActorLocation();
	EndLocation = GetActorTransform().TransformPosition(EndPlatformLocation);

	if (IsValid(PlatformTrigger))
	{
		PlatformTrigger->OnTriggerActivated.AddDynamic(this, &ABasePlatform2::OnPlatformTriggerActivated);
	}
}

void ABasePlatform2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform2::MovePlatform()
{
	if (bIsMovingForward)
	{
		PlatformTimeline.Reverse();
		bIsMovingForward = false;
	}
	else
	{
		PlatformTimeline.Play();
		bIsMovingForward = true;
	}
}

void ABasePlatform2::OnPlatformTriggerActivated(bool bIsActivated)
{
	MovePlatform();
}

void ABasePlatform2::TickPlatformTimeline(float Value)
{
	FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, Value); 
	SetActorLocation(NewLocation);
}

void ABasePlatform2::OnPlatfromEndReached()
{
	if (PlatformBehavior == EPlatformBehavior2::Loop)
	{
		MovePlatform();
	}
	else if (ReturnTime > 0.0f )
	{
		GetWorld()->GetTimerManager().SetTimer(ReturnTimer, this, &ABasePlatform2::MovePlatform, ReturnTime, false);
		return;
	}
}

