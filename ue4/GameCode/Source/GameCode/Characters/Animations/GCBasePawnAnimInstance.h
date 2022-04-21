// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GCBasePawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UGCBasePawnAnimInstance : public UAnimInstance
{
    GENERATED_BODY()
protected:
    TWeakObjectPtr<class AGameCodeBasePawn> CachedBasePawn;
    UPROPERTY(BlueprintReadonly, Transient, Category = "BasePawnAnimationInstance")
    float InputForward;
    UPROPERTY(BlueprintReadonly, Transient, Category = "BasePawnAnimationInstance")
    float InputRight;
    UPROPERTY(BlueprintReadonly, Transient, Category = "BasePawnAnimationInstance")
    bool bIsInAir;
public:
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

};
