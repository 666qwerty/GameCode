// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacterMovementComponent.h"
#include "../GCBaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "../../GameCodeTypes.h"
#include "../../Actors/Interactive/Ladder.h"
#include "../../Actors/Interactive/Zipline.h"
#include "Net/UnrealNetwork.h"

UGCBaseCharacterMovementComponent::UGCBaseCharacterMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	SetIsReplicatedByDefault(true);
	// ...
}

void UGCBaseCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGCBaseCharacterMovementComponent, bIsSliding);
	DOREPLIFETIME(UGCBaseCharacterMovementComponent, bIsSprinting);
}

void UGCBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	/*FLAG_Reserved_1 = 0x04,	// Reserved for future use
		FLAG_Reserved_2 = 0x08,	// Reserved for future use
		// Remaining bit masks are available for custom flags.
		FLAG_Custom_0 = 0x10,
		FLAG_Custom_1 = 0x20,
		FLAG_Custom_2 = 0x40,
		FLAG_Custom_3 = 0x80,*/
	bool bWasMantling = GetBaseCharacterOwner()->bIsMantling;
	bool bIsMantling = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
	bIsSliding = (Flags & FSavedMove_Character::FLAG_Custom_2) != 0;
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_Authority)
	{
		if (bIsMantling)
		{
			GetBaseCharacterOwner()->Mantle();
		}
		if (bIsSliding && CustomMovementMode != (uint8)ECustomMovementMode::CMOVE_Slide)
		{
			StartSlide();
		}
		if (!bIsSliding && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Slide)
		{
			StopSlide();
		}
		if ((Flags & FSavedMove_Character::FLAG_Custom_0) && !bIsSprinting)
		{
			GetBaseCharacterOwner()->StartSprint();
		}
		if (!(Flags & FSavedMove_Character::FLAG_Custom_0) && bIsSprinting)
		{
			GetBaseCharacterOwner()->StopSprint();
		}
	}
}

FNetworkPredictionData_Client* UGCBaseCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UGCBaseCharacterMovementComponent* MutableThis = const_cast<UGCBaseCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Character_GC(*this);
	}
	return ClientPredictionData;
}

float UGCBaseCharacterMovementComponent::GetMaxSpeed() const
{
	if (bIsWallRunning)
	{
		return MaxWallRunSpeed;
	}
    if (bIsSprinting)
    {
	    return SprintSpeed;
    }
    if (bIsProning)
    {
	    return ProningSpeed;
    }
    if (IsOnLadder())
    {
	    return ClimbingLadderSpeed;
    }
    if (bIsOutOfStamina)
    {
	    return OutOfStaminaSpeed;
    }
	if (GetBaseCharacterOwner()->IsAim())
	{
		return GetBaseCharacterOwner()->GetAimingMovementSpeed();
	}
    return Super::GetMaxSpeed();
}

void UGCBaseCharacterMovementComponent::StartSprint()
{
    bForceMaxAccel = true;
	bIsSprinting = true;
}

void UGCBaseCharacterMovementComponent::StopSprint()
{
    bForceMaxAccel = false;
	bIsSprinting = false;
}

void UGCBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		const float AngleTolerance = 1e-3f;
		FRotator DesiredRotation = ForceTargetRotation;
		if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
		{
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation, /*bSweep*/ false );
			return;
		}
		else
		{
			bForceRotation = false;
		}
	}
	if (IsOnLadder() || IsSliding())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

bool UGCBaseCharacterMovementComponent::IsSurfaceWallRunable(const FVector& SurfaceNormal) const
{
	return !(SurfaceNormal.Z > GetWalkableFloorZ() || SurfaceNormal.Z < -0.005f);
}

void UGCBaseCharacterMovementComponent::OnRep_IsSliding(bool bWasSliding)
{
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy && bIsSliding != bWasSliding)
	{
		if (bIsSliding)
		{
			StartSlide();
		}
		else
		{
			StopSlide();
		}
	}
}

void UGCBaseCharacterMovementComponent::OnRep_IsSprinting(bool bWasSprinting)
{
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy && bIsSprinting != bWasSprinting)
	{
		if (bIsSprinting)
		{
			GetBaseCharacterOwner()->StartSprint();
		}
		else
		{
			GetBaseCharacterOwner()->StopSprint();
		}
	}
}

void UGCBaseCharacterMovementComponent::GetWallRunSideAndDirection(const FVector& HitNormal, EWallRunSide& OutSide, FVector& OutDirection) const
{
	if (FVector::DotProduct(HitNormal, GetOwner()->GetActorRightVector()) > 0)
	{
		OutSide = EWallRunSide::Left;
		OutDirection = FVector::CrossProduct(HitNormal, FVector::UpVector).GetSafeNormal();
	}
	else
	{
		OutSide = EWallRunSide::Right;
		OutDirection = FVector::CrossProduct(FVector::UpVector, HitNormal).GetSafeNormal();
	}
}

void UGCBaseCharacterMovementComponent::OnPlayerCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector HitNormal = Hit.ImpactNormal;

	if (bIsWallRunning || !IsSurfaceWallRunable(HitNormal) || !IsFalling())
	{
		return;
	}

	EWallRunSide Side = EWallRunSide::None;
	FVector Direction = FVector::ZeroVector;
	GetWallRunSideAndDirection(HitNormal, Side, Direction);

	if (!GetBaseCharacterOwner()->AreRequiredKeysDown(Side) || FVector::DotProduct(LastWallRunNormal, HitNormal) > 0)
	{
		return;
	}

	StartWallRun(Side, Direction);
}

void UGCBaseCharacterMovementComponent::StartWallRun(EWallRunSide Side, const FVector& Direction)
{
	CurrentWallRunTime = 0;
	bIsWallRunning = true;
	CurrentWallRunSide = Side;
	CurrentWallRunDirection = Direction;
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_WallRun);
}

void UGCBaseCharacterMovementComponent::StopWallRun()
{
	bIsWallRunning = false;
	CurrentWallRunSide = EWallRunSide::None;
	SetMovementMode(MOVE_Falling);
}

void UGCBaseCharacterMovementComponent::Prone()
{		
    const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ProneCapsuleHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ProneCapsuleHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
    UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	bIsProning = true;
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ProneCapsuleHalfHeight);
    Cast<AGCBaseCharacter>(CharacterOwner)->OnProned(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool UGCBaseCharacterMovementComponent::CrouchBool()
{
	bool OldCrouched = CharacterOwner->bIsCrouched;
	CharacterOwner->bIsCrouched = false;
	Crouch();
	bool Result = CharacterOwner->bIsCrouched;
	if (OldCrouched) 
	{
		CharacterOwner->bIsCrouched = true;
	}
	return Result;
}

void UGCBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(ProneCapsuleRadius, ProneCapsuleHalfHeight);
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
	}
	
	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UGCBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
		}
		default:
			break;
		}
	}
	
	if (PreviousMovementMode == MOVE_Custom)
	{
		switch (PreviousCustomMode)
		{
		case (uint8)ECustomMovementMode::CMOVE_Ladder:
		{
				CurrentLadder = nullptr;
		}
		default:
			break;
		}
	}

	if (MovementMode == MOVE_Walking)
	{
		LastWallRunNormal = FVector::ZeroVector;
	}
}

void UGCBaseCharacterMovementComponent::UnProne()
{
	if(CrouchBool()) 
	{
		bIsProning = false;
	}
}

void UGCBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (bWantToProne && !IsProning())
	{
		Prone();
	}
	else if (!bWantToProne && IsProning())
	{
		UnProne();
	}
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UGCBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;
	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);
	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;
	
	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);
	
	FVector LedgeComponentDeltaLocation = CurrentMantlingParameters.Component->GetComponentLocation() - CurrentMantlingParameters.LedgeInitialLocation;
	FRotator LedgeComponentDeltaRotation = CurrentMantlingParameters.Component->GetComponentRotation() - CurrentMantlingParameters.LedgeInitialRotation;

	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation, PositionAlpha) + LedgeComponentDeltaLocation;
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha) + LedgeComponentDeltaRotation;
	FVector Delta = NewLocation - GetActorLocation();
	Velocity = Delta / DeltaTime;
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UGCBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1, false, ClimbingOnLadderBreakingDeceleration);
	FVector Delta = Velocity * DeltaTime;

	if (HasAnimRootMotion())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToLadderProjection(NewPos);

	if (NewPosProjection < CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if ((NewPosProjection > CurrentLadder->GetHeight() - CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) &&
		FVector::DotProduct(Delta, CurrentLadder->GetActorUpVector()) > 0)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
}

void UGCBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iterations)
{
	Velocity = CurrentZipline->GetWireDirection() * ZiplineSpeed;
	
	if(!CurrentZipline->IsInverted())
	{
		Velocity = -Velocity;
	}

	FVector Delta = Velocity * DeltaTime;

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetActorToZiplineProjection(NewPos);

	if (NewPosProjection < ZiplineSpeed / 2)
	{
		DetachFromZipline();
		return;
	}
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
}

bool UGCBaseCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsSliding();
}

void UGCBaseCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	Velocity = GetOwner()->GetActorForwardVector() * SlideSpeed;
	FVector Delta = Velocity * DeltaTime;

	FHitResult Hit;	
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
	if (Hit.ImpactNormal != FVector::ZeroVector)
	{
		StopSlide();
	}
}

void UGCBaseCharacterMovementComponent::PhysWallRun(float DeltaTime, int32 Iterations)
{
	CurrentWallRunTime += DeltaTime;
	if (!GetBaseCharacterOwner()->AreRequiredKeysDown(CurrentWallRunSide) || CurrentWallRunTime > MaxWallRunTime)
	{
		StopWallRun();
		return;
	}

	FHitResult HitResult;

	FVector LineTraceDirection = CurrentWallRunSide == EWallRunSide::Right ? GetOwner()->GetActorRightVector() : -GetOwner()->GetActorRightVector();
	float LineTraceLength = 50.0f;

	FVector StartPosition = GetActorLocation();
	FVector EndPosition = StartPosition + LineTraceLength * LineTraceDirection;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	bool bStableWall = false;

	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	if (GetWorld()->SweepSingleByChannel(HitResult, StartPosition, EndPosition, FQuat::Identity, ECC_WallRunnable, CollisionShape, QueryParams))
	{
		EWallRunSide Side = EWallRunSide::None;
		FVector Direction = FVector::ZeroVector;
		bStableWall = (LastWallRunNormal - HitResult.ImpactNormal).IsNearlyZero();
		LastWallRunNormal = HitResult.ImpactNormal;
		GetWallRunSideAndDirection(HitResult.ImpactNormal, Side, Direction);

		if (Side != CurrentWallRunSide)
		{
			StopWallRun();
			return;
		}
		else
		{
			CurrentWallRunDirection = Direction;
			Velocity = GetMaxSpeed() * CurrentWallRunDirection;
		}
	}
	else
	{
		StopWallRun();
		return;
	}
	FHitResult Hit;
	FVector Delta = Velocity * DeltaTime;

	if (bStableWall) {
		FVector WallNormal = FVector(LastWallRunNormal.X, LastWallRunNormal.Y, 0).GetSafeNormal();
		float DistanceToWall = FMath::Abs(FVector::DotProduct(HitResult.ImpactPoint - GetOwner()->GetActorLocation(), WallNormal));
		Delta = Delta + (CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() * 1.2 - DistanceToWall) * WallNormal;
	}

	ForceTargetRotation = CurrentWallRunDirection.ToOrientationRotator();
	bForceRotation = true;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
	if (FVector::DotProduct(LastWallRunNormal, Hit.ImpactNormal))
	{
		LastWallRunNormal = (LastWallRunNormal + Hit.ImpactNormal).GetSafeNormal();
		StopWallRun();
	}
}
AGCBaseCharacter* UGCBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<AGCBaseCharacter*>(CharacterOwner);
}

void UGCBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		return;
	}
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
	{
		PhysMantling(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_Ladder:
	{
		PhysLadder(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_Zipline:
	{
		PhysZipline(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_WallRun:
	{
		PhysWallRun(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_Slide:
	{
		PhysSlide(DeltaTime, Iterations);
		break;
	}
	default:
		break;
	}
	Super::PhysCustom(DeltaTime, Iterations);
}

void UGCBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingMovementParameters)
{
	CurrentMantlingParameters = MantlingMovementParameters;
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UGCBaseCharacterMovementComponent::EndMantle()
{
	GetBaseCharacterOwner()->bIsMantling = false;
	SetMovementMode(MOVE_Walking);
}

bool UGCBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

bool UGCBaseCharacterMovementComponent::IsSliding() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Slide;
}

float UGCBaseCharacterMovementComponent::GetActorToLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("GetActorToLadderProjection empty Ladder"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

float UGCBaseCharacterMovementComponent::GetActorToZiplineProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentZipline), TEXT("GetActorToZiplineProjection empty Zipline"));
	FVector ZiplineRightVector = CurrentZipline->GetActorRightVector();
	FVector ZiplineToCharacterDistance = Location - CurrentZipline->GetActorLocation();
	return FVector::DotProduct(ZiplineRightVector, ZiplineToCharacterDistance);
}

void UGCBaseCharacterMovementComponent::AttachToLadder(const class ALadder* Ladder)
{
	CurrentLadder = Ladder;
	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180;
	
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float ActorToLadderProjection = GetActorToLadderProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + ActorToLadderProjection * LadderUpVector + LadderCharacterOffset * LadderForwardVector;

	if (CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}
	GetOwner()->SetActorLocation(NewCharacterLocation);
	ForceTargetRotation = TargetOrientationRotation;
	bForceRotation = true;

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
}

void UGCBaseCharacterMovementComponent::AttachToZipline(const class AZipline* Zipline)
{
	CurrentZipline = Zipline;
	FRotator TargetOrientationRotation = CurrentZipline->GetActorRightVector().ToOrientationRotator();
	if(!CurrentZipline->IsInverted())
	{
		TargetOrientationRotation.Yaw += 180;
	}
	
	FVector ZiplineUpVector = CurrentZipline->GetActorUpVector();
	FVector ZiplineRightVector = CurrentZipline->GetActorRightVector();
	float ActorToZiplineProjection = GetActorToZiplineProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentZipline->GetActorLocation() + ActorToZiplineProjection * ZiplineRightVector + (CurrentZipline->GetHeightFromDistance(ActorToZiplineProjection) - ZiplineCharacterOffset) * FVector::UpVector;
	
	GetOwner()->SetActorLocation(NewCharacterLocation);
	ForceTargetRotation = TargetOrientationRotation;
	bForceRotation = true;

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void UGCBaseCharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

void UGCBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod)
{
	switch (DetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::ReachingTheTop:
		GetBaseCharacterOwner()->Mantle();
		break;
	case EDetachFromLadderMethod::ReachingTheBottom:
		SetMovementMode(MOVE_Walking);
		break;
	case EDetachFromLadderMethod::Jump:
	{
		FVector JumpDirection = CurrentLadder->GetActorForwardVector();
		SetMovementMode(MOVE_Falling);
		FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation = true;
		Launch(JumpVelocity);
		break;
	}
	case EDetachFromLadderMethod::Fall:
	default:
		SetMovementMode(MOVE_Falling);
		break;
	}
}

bool UGCBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

bool UGCBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

float UGCBaseCharacterMovementComponent::GetLadderSpeedRatio() const
{
	checkf(IsValid(CurrentLadder), TEXT("GetLadderSpeedRatio no ladder"));
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingLadderSpeed;
}

void UGCBaseCharacterMovementComponent::JumpFromWall()
{
	SetMovementMode(MOVE_Falling);
	FVector JumpVelocity = (LastWallRunNormal + FVector::UpVector).GetSafeNormal() * JumpOffFromLadderSpeed;
	ForceTargetRotation = LastWallRunNormal.ToOrientationRotator();
	bForceRotation = true;
	Launch(JumpVelocity);
	StopWallRun();
}

void UGCBaseCharacterMovementComponent::StartSlide()
{
	if (!IsSprinting() || IsSliding())
	{
		return;
	}
	GetBaseCharacterOwner()->bUseControllerRotationYaw = false;
	GetWorld()->GetTimerManager().SetTimer(SlidingTimer, this, &UGCBaseCharacterMovementComponent::StopSlide, SlideMaxTime, false);
	GetBaseCharacterOwner()->Crouch();
	Crouch();
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Slide);
	GetBaseCharacterOwner()->PlaySlideAnimation();
}

void UGCBaseCharacterMovementComponent::StopSlide()
{
	SetMovementMode(MOVE_Walking);
	GetBaseCharacterOwner()->bUseControllerRotationYaw = true;
	GetBaseCharacterOwner()->UnCrouch();
}

void FSavedMove_GC::Clear()
{
	Super::Clear();
	bSavedIsSprinting = 0;
	bSavedIsMantling = 0;
	bSavedIsSliding = 0;
}

uint8 FSavedMove_GC::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	/*FLAG_Reserved_1 = 0x04,	// Reserved for future use
		FLAG_Reserved_2 = 0x08,	// Reserved for future use
		// Remaining bit masks are available for custom flags.
		FLAG_Custom_0 = 0x10,
		FLAG_Custom_1 = 0x20,
		FLAG_Custom_2 = 0x40,
		FLAG_Custom_3 = 0x80,*/
	if (bSavedIsMantling)
	{
		Result |= FLAG_Custom_1;
		Result &= ~FLAG_JumpPressed;
	}
	if (bSavedIsSprinting)
	{
		Result |= FLAG_Custom_0;
	}
	if (bSavedIsSliding)
	{
		Result |= FLAG_Custom_2;
	}
	return Result;
}

void FSavedMove_GC::SetMoveFor(ACharacter* InCharacter, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(InCharacter, InDeltaTime, NewAccel, ClientData);
	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(InCharacter->GetMovementComponent());
	bSavedIsSprinting = MovementComponent->IsSprinting();
	bSavedIsSliding = MovementComponent->IsSliding();
	bSavedIsMantling = StaticCast<AGCBaseCharacter*>(InCharacter)->bIsMantling;
}

void FSavedMove_GC::PrepMoveFor(ACharacter* InCharacter)
{
	Super::PrepMoveFor(InCharacter);
	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(InCharacter->GetMovementComponent());
	//MovementComponent->bIsSprinting = bSavedIsSprinting;
}

bool FSavedMove_GC::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_GC* NewMove = StaticCast<const FSavedMove_GC*>(NewMovePtr.Get());

	if (bSavedIsSliding != NewMove->bSavedIsSliding)
	{
		return false;
	}
	if (bSavedIsSprinting != NewMove->bSavedIsSprinting)
	{
		return false;
	}
	if (bSavedIsMantling != NewMove->bSavedIsMantling)
	{
		return false;
	}
	return Super::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);
}

FSavedMovePtr FNetworkPredictionData_Client_Character_GC::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_GC);
}

FNetworkPredictionData_Client_Character_GC::FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement)
	: FNetworkPredictionData_Client_Character(ClientMovement)
{

}