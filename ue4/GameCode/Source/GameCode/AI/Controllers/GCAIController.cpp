// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/GCAIController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense.h"
#include "Perception/AIPerceptionComponent.h"

AGCAIController::AGCAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

AActor* AGCAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const
{
	if (!IsValid(GetPawn()))
	{
		return nullptr;
	}
	TArray<AActor*> SensedActors;
	PerceptionComponent->GetCurrentlyPerceivedActors(SenseClass, SensedActors);
	AActor* ClosestActor = nullptr;
	float MinSqueredDistance = FLT_MAX;
	FVector PawnLocation = GetPawn()->GetActorLocation();
	for (AActor* SeenActor : SensedActors)
	{
		float CurrentDistance = FVector::DistSquared(PawnLocation, SeenActor->GetActorLocation());
		if (CurrentDistance < MinSqueredDistance)
		{
			MinSqueredDistance = CurrentDistance;
			ClosestActor = SeenActor;
		}
	}
	return ClosestActor;
}

void AGCAIController::SetIgnoreMoveInput(bool bNewMoveInput)
{
	if (bNewMoveInput)
	{
		RequestId = FAIRequestID::CurrentRequest;
		//PauseMove(RequestId);
	}
	else
	{
		//ResumeMove(RequestId);
	}
}
