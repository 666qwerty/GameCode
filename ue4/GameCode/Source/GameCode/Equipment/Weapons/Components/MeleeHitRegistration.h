// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MeleeHitRegistration.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMeleeRegistered, const FHitResult&, const FVector&)
/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GAMECODE_API UMeleeHitRegistration : public USphereComponent
{
	GENERATED_BODY()
public:
	UMeleeHitRegistration();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	void ProcessHitRegistration();
	void SetIsHitRegistrationEnabled(bool bIsEnabled_In) { bIsHitRegistrationEnabled = bIsEnabled_In; }
	FOnMeleeRegistered OnMeleeHitRegistered;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsHitRegistrationEnabled = false;
private:
	FVector PreviousComponentLocation = FVector::ZeroVector;
};
