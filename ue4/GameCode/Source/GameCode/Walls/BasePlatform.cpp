// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"
#include "PlatformInvocator.h"

// Sets default values
ABasePlatform::ABasePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

// Called when the game starts or when spawned
void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = PlatformMesh->GetRelativeLocation();	

	if (IsValid(TimelineCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
		PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);
		if(PlatformBehavior == EPlatformBehavior::Loop)
		{
			StartMovingWithDelay(MovementCooldown);
		}
	}

	if (IsValid(PlatformInvocator))
	{
		PlatformInvocator->OnInvoctorActivated.AddUObject(this, &ABasePlatform::StartMoving);
	}
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::PlatformTimelineUpdate(const float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
	if (Alpha == 0)
	{
		CurrentPosition = EPlatformPosition::Start;
		if (PlatformBehavior == EPlatformBehavior::Loop) 
		{
			StartMovingWithDelay(MovementCooldown);
		}
	}
	else if (Alpha == 1)
	{
		CurrentPosition = EPlatformPosition::End;
		if (PlatformBehavior == EPlatformBehavior::Loop)
		{
			StartMovingWithDelay(MovementCooldown);
		}
	}
	else
	{
		CurrentPosition = EPlatformPosition::Moving;
	}
}

void ABasePlatform::StartMoving()
{
	if (CurrentPosition == EPlatformPosition::Start)
	{
		PlatformTimeline.Play();
	}
	else if (CurrentPosition == EPlatformPosition::End)
	{
		PlatformTimeline.Reverse();
	}
}

void ABasePlatform::StartMovingWithDelay(float Delay)
{
	GetWorld()->GetTimerManager().SetTimer(PlatformMovementTimer, this, &ABasePlatform::StartMoving, Delay, false);
}

