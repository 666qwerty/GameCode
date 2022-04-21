// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrollingComponent.generated.h"

class APatrollingPath;

UENUM(BlueprintType)
enum class EPatrollingPattern : uint8
{
	Circle = 0,
	PingPongForward = 1,
	PingPongBackward = 2,
};

USTRUCT(BlueprintType)
struct FPathDesctription
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	APatrollingPath* PatrollingPath;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	EPatrollingPattern PatrollingPattern = EPatrollingPattern::Circle;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UAIPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIPatrollingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector SelectClosestWaypoint();
	FVector SelectNextWaypoint();
	bool CanPatrol();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	FPathDesctription PathDesctription;
private:	
	int32 CurrentWaypointIndex = -1;
	// Called every frame

		
};
