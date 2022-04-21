// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Navigation/PatrollingPath.h"

// Sets default values
APatrollingPath::APatrollingPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APatrollingPath::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APatrollingPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

