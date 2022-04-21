// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacterAnimInstance.h"
#include "../GCBaseCharacter.h"
#include "../Components/GCBaseCharacterMovementComponent.h"
#include "../Components/CharacterEquipmentComponent.h"
#include "Equipment/Weapons/RangeWeaponItem.h"
#include "../GCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGCBaseCharacterAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();
    checkf(TryGetPawnOwner()->IsA<AGCBaseCharacter>(), TEXT("AGCBaseCharacter not match it's animation"));
    CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(TryGetPawnOwner());
}

void UGCBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);
    if (!CachedBaseCharacter.IsValid())
    {
        return;
    }
    bIsFalling = CachedBaseCharacter->GetCharacterMovement()->IsFalling();
    bIsCrouching = CachedBaseCharacter->GetCharacterMovement()->IsCrouching();
    bIsProning = CachedBaseCharacter->GetGCMovementComponent()->IsProning();
    bIsStanding = CachedBaseCharacter->GetGCMovementComponent()->IsStanding();
    bIsOutOfStamina = CachedBaseCharacter->GetGCMovementComponent()->IsOutOfStamina();
    bIsSprinting = CachedBaseCharacter->GetGCMovementComponent()->IsSprinting();
    bIsSwimming = CachedBaseCharacter->GetCharacterMovement()->IsSwimming();
    bIsOnLadder = CachedBaseCharacter->GetGCMovementComponent()->IsOnLadder();
    bIsOnZipline = CachedBaseCharacter->GetGCMovementComponent()->IsOnZipline();
    bIsAim = CachedBaseCharacter->IsAim();
    bIsStrafing = !CachedBaseCharacter->GetCharacterMovement()->bOrientRotationToMovement;
    AimRotation = CachedBaseCharacter->GetAimOffset();

    CurrentEquippedType = CachedBaseCharacter->GetGCEquipmentComponent()->GetEquippedItemType();
    switch (CachedBaseCharacter->GetGCMovementComponent()->GetWallRunSide())
    {
    case EWallRunSide::Left:
        bIsOnLeftWallRun = true;
        bIsOnRightWallRun = false;
        break;
    case EWallRunSide::Right:
        bIsOnLeftWallRun = false;
        bIsOnRightWallRun = true;
        break;
    case EWallRunSide::None:
    default:
        bIsOnLeftWallRun = false;
        bIsOnRightWallRun = false;
        break;
    }
    Speed = CachedBaseCharacter->GetCharacterMovement()->Velocity.Size();
    Direction = CalculateDirection(CachedBaseCharacter->GetCharacterMovement()->Velocity, CachedBaseCharacter->GetActorRotation());
    if(bIsOnLadder)
    {
        LadderSpeedRatio = CachedBaseCharacter->GetGCMovementComponent()->GetLadderSpeedRatio();
    }
    PelvisEffectorLocation = FVector(0, 0, -CachedBaseCharacter->GetIKPelvisOffset());
    RightFootEffectorLocation = FVector(CachedBaseCharacter->GetIKRightFootOffset() - CachedBaseCharacter->GetIKPelvisOffset(), 0, 0);
    LeftFootEffectorLocation = -FVector(CachedBaseCharacter->GetIKLeftFootOffset() - CachedBaseCharacter->GetIKPelvisOffset(), 0, 0);
    ARangeWeaponItem* CurrentRangeWeapon = CachedBaseCharacter->GetGCEquipmentComponent()->GetCurrentRangeWeapon();
    if (IsValid(CurrentRangeWeapon))
    {
        ForeGripSocketTransform = CurrentRangeWeapon->GetForeGripTransform();
    }
}
