// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/AITurretController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AIPerceptionComponent.h"
#include "AI/Characters/Turret.h"

void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController has wrong owner"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!CachedTurret.IsValid())
	{
		return;
	}
	bool bHasSeenActors = true;
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (ClosestActor == nullptr)
	{
		bHasSeenActors = false;
		ClosestActor = GetClosestSensedActor(UAISense_Damage::StaticClass());
	}

	CachedTurret->CurrentTarget = ClosestActor;
	CachedTurret->bHasSeenTarget = bHasSeenActors;
	CachedTurret->OnCurrentTargetSet();
}
