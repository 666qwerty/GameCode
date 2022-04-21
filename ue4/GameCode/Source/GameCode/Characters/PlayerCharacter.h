// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCBaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

class AEquipableItem;
/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API APlayerCharacter : public AGCBaseCharacter
{
	GENERATED_BODY()
public:
    APlayerCharacter(const FObjectInitializer& ObjectInitializer);
    virtual void SetForwardAxis(float Value) { ForwardAxis = Value; }
    virtual void SetRightAxis(float Value) { RightAxis = Value; }
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override; 
protected:
    // Called when the game starts or when spawned
    virtual void MoveForward(float Value) override;
    virtual void MoveRight(float Value) override;
    virtual void SwimForward(float Value) override;
    virtual void SwimRight(float Value) override;
    virtual void SwimUp(float Value) override;
    virtual void Turn(float Value) override;
    virtual void LookUp(float Value) override;
    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    bool AreRequiredKeysDown(enum class EWallRunSide Side) const;

    virtual bool CanJumpInternal_Implementation() const override;
    virtual void OnJumped_Implementation() override;
    virtual void OnProned(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter | Camera")
    class USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PlayerCharacter | Camera")
    class UCameraComponent* CameraComponent;
    float ForwardAxis = 0;
    float RightAxis = 0;

    virtual void PossessedBy(AController* NewController) override;
    virtual void OnMontageTimerStarted(float Duration);
    virtual void OnMontageTimerElapsed();
    virtual void OnStartAim_Implementation() override;
    virtual void OnStopAim_Implementation() override;
    TWeakObjectPtr<class AGCPlayerController> PlayerController;

    virtual void AimTimelineUpdate(float Alpha);
    FTimeline MainAimTimeline;
    FTimeline& AimTimeline = MainAimTimeline;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UCurveFloat* AimTimelineCurve;

private:
    UFUNCTION(Client, Reliable)
    void Client_SetController();
};
