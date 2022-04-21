// Fill out your copyright notice in the Description page of Project Settings.

#include "FPPlayerAnimInstance.h"
#include "../FPPlayerCharacter.h"
#include "../Components/GCBaseCharacterMovementComponent.h"
#include "../Controllers/GCPlayerController.h"
#include "Equipment/Weapons/RangeWeaponItem.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFPPlayerAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("AFPPlayerCharacter not match it's animation"));
    CachedFPCharacterOwner = StaticCast<AFPPlayerCharacter*>(TryGetPawnOwner());
}

void UFPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!CachedFPCharacterOwner.IsValid())
    {
        return;
    }
    PlayerCameraPitchAngle = CalculateCameraPitchAngle();
   
}

float UFPPlayerAnimInstance::CalculateCameraPitchAngle() const
{
    float Result = 0;
    AGCPlayerController* Controller = CachedFPCharacterOwner->GetController<AGCPlayerController>();
    if (IsValid(Controller) && !Controller->GetIgnoreCameraPitch())
    {
        Result = Controller->GetControlRotation().Pitch;
    }
    return Result;
}
