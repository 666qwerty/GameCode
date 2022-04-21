// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipableItem.h"
#include "GameCodeTypes.h"
#include "MeleeWeaponItem.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMeleeAttackDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageAmount = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* AttackMontage;
};

class UMeleeHitRegistration;
UCLASS(Blueprintable)
class GAMECODE_API AMeleeWeaponItem : public AEquipableItem
{
	GENERATED_BODY()
public:
	AMeleeWeaponItem();
	void StartAttack(EMeleeAttackType AttackType);
	void SetIsHitRegistratorsEnabled(bool bIsHitRegistratorsEnabled);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EMeleeAttackType, FMeleeAttackDescription> Attacks;

	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& HitDirection);

	TArray<UMeleeHitRegistration*> HitRegistrators;
	TSet<AActor*> HitActors;
	void OnAttackTimerElapsed();
	FTimerHandle AttackTimer;
	FMeleeAttackDescription* CurrentAttack;

};
