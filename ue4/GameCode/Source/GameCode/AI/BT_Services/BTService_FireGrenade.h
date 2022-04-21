// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FireGrenade.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UBTService_FireGrenade : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_FireGrenade();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinDistance = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxDistance = 2000;
};
