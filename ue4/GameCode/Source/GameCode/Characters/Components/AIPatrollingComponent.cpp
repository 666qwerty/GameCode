// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Components/AIPatrollingComponent.h"
#include "Actors/Navigation/PatrollingPath.h"

// Sets default values for this component's properties
UAIPatrollingComponent::UAIPatrollingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAIPatrollingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAIPatrollingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UAIPatrollingComponent::SelectClosestWaypoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector>& Waypoints = PathDesctription.PatrollingPath->GetWaypoints();
	FTransform PathTransform = PathDesctription.PatrollingPath->GetActorTransform();
	FVector ClosestWayPoint;
	float MinSquaredDistance = FLT_MAX;
	for (int i=0; i<Waypoints.Num(); ++i)
	{
		const FVector Waypoint = Waypoints[i];
		FVector WaypointWorld = PathTransform.TransformPosition(Waypoint);
		float CurrentSqDistance = (OwnerLocation - WaypointWorld).SizeSquared();
		if (CurrentSqDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSqDistance;
			ClosestWayPoint = WaypointWorld;
			CurrentWaypointIndex = i;
		}
	}
	return ClosestWayPoint;
}

FVector UAIPatrollingComponent::SelectNextWaypoint()
{
	switch (PathDesctription.PatrollingPattern)
	{
	case EPatrollingPattern::Circle:
	{
		++CurrentWaypointIndex;
		if (CurrentWaypointIndex == PathDesctription.PatrollingPath->GetWaypoints().Num())
		{
			CurrentWaypointIndex = 0;
		}
		break;
	}
	case EPatrollingPattern::PingPongForward:
	{
		++CurrentWaypointIndex;
		if (CurrentWaypointIndex == PathDesctription.PatrollingPath->GetWaypoints().Num())
		{
			CurrentWaypointIndex -= 2;
			PathDesctription.PatrollingPattern = EPatrollingPattern::PingPongBackward;
		}
		break;
	}
	case EPatrollingPattern::PingPongBackward:
	{
		--CurrentWaypointIndex;
		if (CurrentWaypointIndex == -1)
		{
			CurrentWaypointIndex += 2;
			PathDesctription.PatrollingPattern = EPatrollingPattern::PingPongForward;
		}
		break;
	}
	}
	
	FTransform PathTransform = PathDesctription.PatrollingPath->GetActorTransform();
	return PathTransform.TransformPosition(PathDesctription.PatrollingPath->GetWaypoints()[CurrentWaypointIndex]);
}

bool UAIPatrollingComponent::CanPatrol()
{
	return IsValid(PathDesctription.PatrollingPath) && PathDesctription.PatrollingPath->GetWaypoints().Num() > 0;
}

