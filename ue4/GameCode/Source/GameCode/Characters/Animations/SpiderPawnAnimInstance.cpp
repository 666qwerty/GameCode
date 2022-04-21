// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderPawnAnimInstance.h"
#include "../SpiderPawn.h"


void USpiderPawnAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    checkf(TryGetPawnOwner()->IsA<ASpiderPawn>(), TEXT("All bad"));
    CachedSpiderPawn = StaticCast<ASpiderPawn*>(TryGetPawnOwner());
}

void USpiderPawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!CachedSpiderPawn.IsValid())
    {
	return;
    }
    RightFrontFootEffectorLocation = FVector(CachedSpiderPawn->GetIKRightFrontFootOffset(), 0, 0);
    LeftFrontFootEffectorLocation = FVector(CachedSpiderPawn->GetIKLeftFrontFootOffset(), 0, 0);
    RightRearFootEffectorLocation = FVector(CachedSpiderPawn->GetIKRightRearFootOffset(), 0, 0);
    LeftRearFootEffectorLocation = FVector(CachedSpiderPawn->GetIKLeftRearFootOffset(), 0, 0);
}
