// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCBasePawnAnimInstance.h"
#include "SpiderPawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API USpiderPawnAnimInstance : public UGCBasePawnAnimInstance
{
	GENERATED_BODY()

public:
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FVector RightFrontFootEffectorLocation = FVector::ZeroVector;
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FVector LeftFrontFootEffectorLocation = FVector::ZeroVector;
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FVector RightRearFootEffectorLocation = FVector::ZeroVector;
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FVector LeftRearFootEffectorLocation = FVector::ZeroVector;
    TWeakObjectPtr<class ASpiderPawn> CachedSpiderPawn;
};
