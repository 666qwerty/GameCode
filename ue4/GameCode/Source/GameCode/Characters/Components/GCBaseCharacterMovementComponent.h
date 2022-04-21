// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LedgeDetectionComponent.h"
#include "GCBaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
    FVector InitialLocation = FVector::ZeroVector;
    FRotator InitialRotation = FRotator::ZeroRotator;
    FVector LedgeInitialLocation = FVector::ZeroVector;
    FRotator LedgeInitialRotation = FRotator::ZeroRotator;
    TWeakObjectPtr<class UPrimitiveComponent> Component;
    FVector TargetLocation = FVector::ZeroVector;
    FRotator TargetRotation = FRotator::ZeroRotator;
    FVector InitialAnimationLocation = FVector::ZeroVector;
    float Duration = 1;
    float StartTime = 0;
    UCurveVector* MantlingCurve;
};

UENUM(BlueprintType)
enum class ECustomMovementMode:uint8
{
    CMOVE_None = 0 UMETA(DisplayName = "None"),
    CMOVE_Mantling = 1 UMETA(DisplayName = "Mantling"),
    CMOVE_Ladder = 2 UMETA(DisplayName = "Ladder"),
    CMOVE_Zipline = 3 UMETA(DisplayName = "Zipline"),
    CMOVE_WallRun = 4 UMETA(DisplayName = "Wall Run"),
    CMOVE_Slide = 5 UMETA(DisplayName = "Slide"),
    CMOVE_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod:uint8
{
    Fall = 0,
    ReachingTheTop = 1,
    ReachingTheBottom = 2,
    Jump = 3
};

UENUM()
enum class EWallRunSide : uint8
{
    None = 0,
    Left = 1,
    Right = 2
};

/**
 * 
 */
UCLASS()
class GAMECODE_API UGCBaseCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

    friend class FSavedMove_GC;

public:
    UGCBaseCharacterMovementComponent();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void UpdateFromCompressedFlags(uint8 Flags);
    virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
    bool IsSprinting() { return bIsSprinting; };
    virtual float GetMaxSpeed() const override;
    virtual bool IsMovingOnGround() const override;
    virtual void PhysicsRotation(float DeltaTime) override;

    void StartSprint();
    void StopSprint();
    void StartSlide();
    void StopSlide();
    void PhysSlide(float DeltaTime, int32 Iterations);
    bool IsSliding() const;
    bool IsCrouching() const { return Super::IsCrouching() && !IsSliding(); };
    
    bool IsOutOfStamina() const { return bIsOutOfStamina; };
    void SetIsOutOfStamina(bool bIsOutOfStamina_In) { bIsOutOfStamina = bIsOutOfStamina_In; };
	bool IsProning() const { return bIsProning; };
    void TryProne(bool bWantToProne_In) { bWantToProne = bWantToProne_In; };
    bool IsStanding() const { return !IsProning() && !IsCrouching(); };
    void Prone();
    void UnProne();
    void StartMantle(const FMantlingMovementParameters& MantlingMovementParameters);
    void EndMantle();
    bool IsMantling() const;    
    void PhysMantling(float DeltaTime, int32 Iterations);
    void AttachToLadder(const class ALadder* Ladder);
    void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
    bool IsOnLadder() const;   
    void PhysLadder(float DeltaTime, int32 Iterations);
    const class ALadder* GetCurrentLadder() { return CurrentLadder; }
    float GetLadderSpeedRatio() const;
    void AttachToZipline(const class AZipline* Ladder);
    void DetachFromZipline();
    bool IsOnZipline() const;   
    void PhysZipline(float DeltaTime, int32 Iterations);
    void StartWallRun(EWallRunSide Side, const FVector& Direction);
    void JumpFromWall();
    void StopWallRun();
    EWallRunSide GetWallRunSide() const { return CurrentWallRunSide; };
    void PhysWallRun(float DeltaTime, int32 Iterations);
    UFUNCTION()
    void OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement")
    float ProneCapsuleHalfHeight = 40.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement")
    float ProneCapsuleRadius = 40.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement:Ladder")
    float LadderCharacterOffset = 60;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement:Ladder")
    float JumpOffFromLadderSpeed = 500;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement:Zipline")
    float ZiplineCharacterOffset = 130;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement: Wall run")
    float MaxWallRunTime = 2;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterMovement: Wall run")
    float MaxWallRunSpeed = 1000;
    UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
    float SlideSpeed = 1000.0f;
    UPROPERTY(Category = "Character Movement: Slide", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
    float SlideMaxTime = 200.0f;

    FRotator GetForceRotation() { return ForceTargetRotation; };


protected:
    virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
    virtual bool CrouchBool();
    virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement")
    float SprintSpeed = 1200;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement")
    float OutOfStaminaSpeed = 200;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement")
    float ProningSpeed = 100;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement:Ladder")
    float ClimbingLadderSpeed = 200;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CharacterMovement:Ladder")
    float ZiplineSpeed = 500;
    virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

    class AGCBaseCharacter* GetBaseCharacterOwner() const;

private:
    float GetActorToLadderProjection(const FVector& Location) const;
    float GetActorToZiplineProjection(const FVector& Location) const;

    void GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutDirection) const;

    bool IsSurfaceWallRunable(const FVector& SurfaceNormal) const;

    UPROPERTY(ReplicatedUsing = OnRep_IsSliding)
    bool bIsSliding = false;
    UFUNCTION()
    void OnRep_IsSliding(bool bWasSliding);
    UPROPERTY(ReplicatedUsing = OnRep_IsSprinting)
    bool bIsSprinting = false;
    UFUNCTION()
    void OnRep_IsSprinting(bool bWasSprinting);
    bool bIsOutOfStamina = false;
    bool bIsProning = false;
    bool bIsWallRunning = false;
	bool bWantToProne = false;
    EWallRunSide CurrentWallRunSide = EWallRunSide::None;
    FVector CurrentWallRunDirection = FVector::ZeroVector;
    float ClimbingOnLadderBreakingDeceleration = 2048;
    FTimerHandle MantlingTimer;
    FMantlingMovementParameters CurrentMantlingParameters;
    const class ALadder* CurrentLadder = nullptr;
    const class AZipline* CurrentZipline = nullptr;
    FRotator ForceTargetRotation = FRotator::ZeroRotator;
    bool bForceRotation = false;
    float CurrentWallRunTime = 0;
    FVector LastWallRunNormal = FVector::ZeroVector;
    FTimerHandle SlidingTimer;
};

class FSavedMove_GC : public FSavedMove_Character
{
    typedef FSavedMove_Character Super;
public:
    virtual void Clear() override;
    virtual uint8 GetCompressedFlags() const override;
    virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;
    virtual void SetMoveFor(ACharacter* InCharacter, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
    virtual void PrepMoveFor(ACharacter* InCharacter) override;
private:
    uint8 bSavedIsSprinting : 1;
    uint8 bSavedIsMantling : 1;
    uint8 bSavedIsSliding : 1;
};

class  FNetworkPredictionData_Client_Character_GC : public FNetworkPredictionData_Client_Character
{
    typedef FNetworkPredictionData_Client_Character Super;

public:
    FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement);
    virtual FSavedMovePtr AllocateNewMove() override;
};