// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameCodeTypes.h"
#include "GCBaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UGCBaseCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    float Speed = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsFalling = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsStanding = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsProning = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsCrouching = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsSprinting = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsOutOfStamina = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsSwimming = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsOnLadder = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsOnZipline = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsOnRightWallRun = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsOnLeftWallRun = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsAim = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    bool bIsStrafing = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    float Direction = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    float LadderSpeedRatio = 0;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    FRotator AimRotation = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
    EEquipableItemType CurrentEquippedType = EEquipableItemType::None;
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FVector RightFootEffectorLocation = FVector::ZeroVector;
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FVector LeftFootEffectorLocation = FVector::ZeroVector;
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FVector PelvisEffectorLocation = FVector::ZeroVector;
    UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly)
    FTransform ForeGripSocketTransform;
private:
    TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
};
