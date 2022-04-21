// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/GCBaseCharacter.h"
#include "GCAICharacter.generated.h"

class UAIPatrollingComponent;
class UBehaviorTree;
/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API AGCAICharacter : public AGCBaseCharacter
{
	GENERATED_BODY()
public:
	AGCAICharacter(const FObjectInitializer& ObjectInitializer);
	UAIPatrollingComponent* GetPatrollingComponent() const { return AIPatrollingComponent; }
	UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPatrollingComponent* AIPatrollingComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UBehaviorTree* BehaviorTree;
};
