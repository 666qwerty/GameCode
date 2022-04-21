// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrollingPath.generated.h"

UCLASS()
class GAMECODE_API APatrollingPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrollingPath();

	const TArray<FVector>& GetWaypoints() const { return Waypoints; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	TArray<FVector> Waypoints;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
