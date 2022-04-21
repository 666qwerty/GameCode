// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT_Services/BTService_FireGrenade.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/GCBaseCharacter.h"
#include "Equipment/Weapons/RangeWeaponItem.h"
#include "Characters/Components/CharacterEquipmentComponent.h"

UBTService_FireGrenade::UBTService_FireGrenade()
{
	NodeName = "FireGrenade";
}

void UBTService_FireGrenade::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(AIController) || !IsValid(Blackboard))
	{
		return;
	}

	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(AIController->GetPawn());
	if (!IsValid(Character))
	{
		return;
	}

	UCharacterEquipmentComponent* EquipmentComponent = Character->GetGCEquipmentComponent();
	ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRangeWeapon();

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!IsValid(CurrentTarget))
	{
		return;
	}
	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistSq > FMath::Square(MaxDistance) || DistSq < FMath::Square(MinDistance))
	{
		return;
	}

	Character->EquipPrimaryItem();
}