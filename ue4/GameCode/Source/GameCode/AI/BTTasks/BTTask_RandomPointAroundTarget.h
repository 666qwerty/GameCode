// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RandomPointAroundTarget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UBTTask_RandomPointAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_RandomPointAroundTarget();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Radius = 500;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector LocationKey;
};
