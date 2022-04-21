// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Characters/GCAICharacter.h"
#include "Characters/Components/AIPatrollingComponent.h"

AGCAICharacter::AGCAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AIPatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("PatrollingComponent"));
}