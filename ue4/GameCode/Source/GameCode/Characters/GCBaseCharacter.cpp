// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Components/GCBaseCharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LedgeDetectionComponent.h"
#include "Components/CharacterAttributesComponent.h"
#include "Components/CharacterEquipmentComponent.h"
#include "Components/CharacterInventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "Equipment/Weapons/RangeWeaponItem.h"
#include "Equipment/Weapons/MeleeWeaponItem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AIController.h"
#include "../Actors/Interactive/Ladder.h"
#include "../Actors/Interactive/Zipline.h"
#include "Actors/Environment/PlatformTrigger.h"
#include "Net/UnrealNetwork.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "GCGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetCharacterAttributes.h"

AGCBaseCharacter::AGCBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UGCBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	MovementComponent = Cast<UGCBaseCharacterMovementComponent>(GetCharacterMovement());
	IKScale = GetActorScale3D().Z;
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LedgeDetectionComponent = CreateDefaultSubobject<ULedgeDetectionComponent>(TEXT("LedgeDetectionComponent"));

	GetMesh()->CastShadow = 1;
	GetMesh()->bCastDynamicShadow = true;

	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>(TEXT("CharacterAttributesComponent"));
	CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("CharacterInventoryComponent"));
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipmentComponent2"));

	HealthbarProgressComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthbarProgressComponent"));
	HealthbarProgressComponent->SetupAttachment(GetCapsuleComponent());
}

void AGCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentHit.AddDynamic(MovementComponent, &UGCBaseCharacterMovementComponent::OnPlayerCapsuleHit);
	CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &AGCBaseCharacter::OnDeath);
	CharacterAttributesComponent->OutOfStaminaEvent.AddUObject(MovementComponent, &UGCBaseCharacterMovementComponent::SetIsOutOfStamina);
	InitializeHealthProgress();
	CharacterEquipmentComponent->CreateLoadout();
}

void AGCBaseCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	if (OnInteractableObjectFound.IsBound())
	{
		OnInteractableObjectFound.Unbind();
	}
	Super::EndPlay(Reason);
}

void AGCBaseCharacter::ChangeCrouchState()
{
	if (!GetCharacterMovement()->IsCrouching() & !MovementComponent->IsSliding())
	{
		Crouch();
	}
}

void AGCBaseCharacter::ChangeProneState()
{
	if (MovementComponent->IsSliding())
	{
		return;
	}
	if (MovementComponent->IsProning())
	{
		MovementComponent->TryProne(false);
	} else if (GetCharacterMovement()->IsCrouching())
	{
		MovementComponent->TryProne(true);
	}
}

void AGCBaseCharacter::StartSprint()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	bIsSprintRequested = true;
}

void AGCBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

bool AGCBaseCharacter::CanMantle() const
{
	return MovementComponent->CanAttemptJump() || MovementComponent->IsSwimming() || MovementComponent->IsOnLadder();
}

bool AGCBaseCharacter::IsUnderWater() const
{
	if (MovementComponent->IsSwimming())
	{
		FVector HeadPosition = GetMesh()->GetSocketLocation(FName("head"));
		APhysicsVolume* Volume = MovementComponent->GetPhysicsVolume();
		float VolumeTopPlane = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z;
		return HeadPosition.Z < VolumeTopPlane;
	}
	return false;
}

void AGCBaseCharacter::Reload()
{
	if (IsValid(CharacterEquipmentComponent))
	{
		CharacterEquipmentComponent->Reload();
	}
}

void AGCBaseCharacter::SecondaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackType::SecondaryAttack);
	}
}

void AGCBaseCharacter::Interact()
{
	if (LineOfSightObject.GetInterface())
	{
		LineOfSightObject->Interact(this);
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_Interact(LineOfSightObject->GetIndex());
		}
	}
}

void AGCBaseCharacter::Server_Interact_Implementation(int32 Index)
{
	UGCGameInstance* GameInstance = StaticCast<UGCGameInstance*>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->InteractableArray[Index]->Interact(this);
}

bool AGCBaseCharacter::PickupItem(TWeakObjectPtr<class UInventoryItem> ItemToPickup)
{
	return CharacterInventoryComponent->AddItem(ItemToPickup, 1);
}

bool AGCBaseCharacter::PickupAmmo(TWeakObjectPtr<class UAmmoInventoryItem> ItemToPickup)
{
	if (CharacterInventoryComponent->AddItem(ItemToPickup, ItemToPickup->Amount))
	{
		CharacterEquipmentComponent->AddAmmo(ItemToPickup->Type, ItemToPickup->Amount);
		return true;
	}
	return false;
}

void AGCBaseCharacter::UseInventory(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return;
	}
	if (!CharacterInventoryComponent->IsViewVisible())
	{
		CharacterInventoryComponent->OpenViewInventory(PlayerController);
		CharacterEquipmentComponent->OpenViewEquipment(PlayerController);
		PlayerController->SetInputMode(FInputModeGameAndUI{});
		PlayerController->bShowMouseCursor = true;
	}
	else
	{
		CharacterInventoryComponent->CloseViewInventory();
		CharacterEquipmentComponent->CloseViewEquipment();
		PlayerController->SetInputMode(FInputModeGameOnly{});
		PlayerController->bShowMouseCursor = false;
	}
}

void AGCBaseCharacter::AddHealth(float Value)
{
	CharacterAttributesComponent->AddHealth(Value);
}

void AGCBaseCharacter::AddStamina(float Value)
{
	CharacterAttributesComponent->AddStamina(Value);
}

void AGCBaseCharacter::PrimaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackType::PrimaryAttack);
	}
}

void AGCBaseCharacter::EquipPrimaryItem()
{
	if (IsValid(CharacterEquipmentComponent))
	{
		CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
	}
}

void AGCBaseCharacter::NextItem()
{
	if (IsValid(CharacterEquipmentComponent))
	{
		CharacterEquipmentComponent->EquipNextSlot();
	}
}

void AGCBaseCharacter::PreviousItem()
{
	if (IsValid(CharacterEquipmentComponent))
	{
		CharacterEquipmentComponent->EquipPreviousSlot();
	}
}


void AGCBaseCharacter::Mantle()
{
	if (!CanMantle())
	{
		return;
	}
	FLedgeDescription LedgeDescription;
	if (LedgeDetectionComponent->DetectLedge(LedgeDescription))
	{
		bIsMantling = true;

		FMantlingMovementParameters MantlingParameters; 
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.Component = LedgeDescription.Component;
		MantlingParameters.LedgeInitialLocation = MantlingParameters.Component->GetComponentLocation();
		MantlingParameters.LedgeInitialRotation = MantlingParameters.Component->GetComponentRotation();
		float ManlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		const FMantlingSettings& MantleSettings = GetMantlingSettings(ManlingHeight);
		MantlingParameters.MantlingCurve = MantleSettings.MantlingCurve;
		float MinRange, MaxRange;
		MantleSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantleSettings.AnimationCorrectionZ * FVector::UpVector + MantleSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;
		
		FVector2D SourceRange(MantleSettings.MinHeight, HeightMantlingSettings.MaxHeight);
		FVector2D TargetRange(MantleSettings.MinHeightStartTime, HeightMantlingSettings.MaxHeightStartTime);

		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, ManlingHeight);
		if (GetLocalRole() > ROLE_SimulatedProxy)
		{
			MovementComponent->StartMantle(MantlingParameters);
		}
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		OnMontageTimerStarted(AnimInstance->Montage_Play(MantleSettings.MantlingMontage, 1, EMontagePlayReturnType::Duration, MantlingParameters.StartTime));
		OnMantle(MantleSettings, MantlingParameters.StartTime);
	}
}

void AGCBaseCharacter::OnMontageTimerStarted(float Duration)
{
	GetWorld()->GetTimerManager().SetTimer(MontageTimer, this, &AGCBaseCharacter::OnMontageTimerElapsed, Duration, false); 
	if (IsValid(Controller))
	{
		Controller->SetIgnoreMoveInput(true);
	}
}


void AGCBaseCharacter::OnMontageTimerElapsed()
{
	if (IsValid(Controller) && CharacterAttributesComponent->IsAlive())
	{
		Controller->SetIgnoreLookInput(false);
		Controller->SetIgnoreMoveInput(false);
	}
}

void AGCBaseCharacter::InitializeHealthProgress()
{
	UWidgetCharacterAttributes* Widget = Cast<UWidgetCharacterAttributes>(HealthbarProgressComponent->GetUserWidgetObject());
	if (!IsValid(Widget))
	{
		HealthbarProgressComponent->SetVisibility(false);
		return;
	}
	if(IsPlayerControlled() && IsLocallyControlled())
	{
		HealthbarProgressComponent->SetVisibility(false);
		return;
	}
	CharacterAttributesComponent->OnHealthChangedEvent.AddUObject(Widget, &UWidgetCharacterAttributes::UpdateAmount);
	CharacterAttributesComponent->OnDeathEvent.AddLambda([=]() {HealthbarProgressComponent->SetVisibility(false); });
	CharacterAttributesComponent->OnHealthChanged();
}

void AGCBaseCharacter::PlaySlideAnimation()
{
	PlayAnimMontage(SlideAnimMontage);
}

void AGCBaseCharacter::InteractWithLadder()
{
	if (MovementComponent->IsOnLadder())
	{
		MovementComponent->DetachFromLadder(EDetachFromLadderMethod::Jump);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if(IsValid(AvailableLadder))
		{
			if(AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			MovementComponent->AttachToLadder(AvailableLadder);
		}
	}
}

void AGCBaseCharacter::InteractWithZipline()
{
	if (MovementComponent->IsOnZipline())
	{
		MovementComponent->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if(IsValid(AvailableZipline))
		{
			MovementComponent->AttachToZipline(AvailableZipline);
		}
	}
}

const ALadder* AGCBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

const AZipline* AGCBaseCharacter::GetAvailableZipline() const
{
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			return StaticCast<const AZipline*>(InteractiveActor);
		}
	}
	return nullptr;
}

void AGCBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (!IsValid(AIController))
	{
		return;
	}
	FGenericTeamId TeamId((uint8)Team);
	AIController->SetGenericTeamId(TeamId);
}

void AGCBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (-GetVelocity().Z > HardLandingSpeed)
	{
		OnMontageTimerStarted(PlayAnimMontage(HardLandingMontage));
	}
	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(-GetVelocity().Z);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

 const FMantlingSettings& AGCBaseCharacter::GetMantlingSettings(float LedgeHeight)
{
	return LedgeHeight > LowMantleMaxHeight ? HeightMantlingSettings : LowMantlingSettings;
}

bool AGCBaseCharacter::CanSprint()
{
	return !MovementComponent->IsOutOfStamina();
}

void AGCBaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy && bIsMantling && !bWasMantling)
	{
		Mantle();
	}
}

void AGCBaseCharacter::ConfirmWeaponSelection()
{
	if (CharacterEquipmentComponent->IsSelectingWeapon())
	{
		CharacterEquipmentComponent->ConfirmWeaponSelection();
	}
}

/*void AGCBaseCharacter::AddEquipmentItem(const TSubclassOf<class AEquipableItem> EquipableItemClass, int32 SlotIndex)
{
	GetGCEquipmentComponent()->AddEquipmentItem(EquipableItemClass, SlotIndex);
}
*/
void AGCBaseCharacter::OnStartSprint_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("OnStringStart"));
}

void AGCBaseCharacter::OnStopSprint_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("OnStringStop"));
}

void AGCBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGCBaseCharacter, bIsMantling);
}

void AGCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TryChangeSprint(DeltaTime);
	UpdateIK(DeltaTime);
	TraceLineOfSight();	
}

void AGCBaseCharacter::UpdateIK(float DeltaTime)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, InterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, InterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, FMath::Max(IKRightFootOffset, IKLeftFootOffset), DeltaTime, InterpSpeed);
}

void AGCBaseCharacter::TryChangeSprint(float DeltaTime)
{
	if (bIsSprintRequested && !MovementComponent->IsSprinting() & CanSprint())
	{
		MovementComponent->StartSprint();
		OnStartSprint();
	}
	else if ((!CanSprint() || !bIsSprintRequested) && MovementComponent->IsSprinting())
	{
		MovementComponent->StopSprint();
		OnStopSprint();
	}
}

void AGCBaseCharacter::ClimbLadderUp(float Value)
{
	if (MovementComponent->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = MovementComponent->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void AGCBaseCharacter::Jump()
{
	if (MovementComponent->GetWallRunSide() != EWallRunSide::None)
	{
		MovementComponent->JumpFromWall();
		CharacterAttributesComponent->JumpTire();
	}
	if (MovementComponent->IsProning())
	{
		MovementComponent->TryProne(false);
		UnCrouch();
	} 
	else if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	} 
	else 
	{
		Super::Jump();
	}
}

void AGCBaseCharacter::StartFire()
{
	if (CharacterEquipmentComponent->IsSelectingWeapon())
	{
		return;
	}
	ARangeWeaponItem* Weapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(Weapon) && !CharacterEquipmentComponent->GetIsEquipping())
	{
		Weapon->StartFire();
	}
}

void AGCBaseCharacter::StopFire()
{
	ARangeWeaponItem* Weapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(Weapon))
	{
		Weapon->StopFire();
	}
}

void AGCBaseCharacter::StartAim()
{
	ARangeWeaponItem* Weapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(Weapon))
	{
		Weapon->StartAim();
	}
}

void AGCBaseCharacter::OnStartAim_Implementation()
{
	ARangeWeaponItem* Weapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(Weapon))
	{
		CurrentAimingMovementSpeed = Weapon->GetAimMaxSpeed();
		bIsAim = true;
		OnAimStateChanged.Broadcast(true);
	}
}

void AGCBaseCharacter::OnStopAim_Implementation()
{
	ARangeWeaponItem* Weapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(Weapon))
	{
		CurrentAimingMovementSpeed = 0;
		bIsAim = false;
		OnAimStateChanged.Broadcast(false);
	}
}

void AGCBaseCharacter::StopAim()
{
	ARangeWeaponItem* Weapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(Weapon))
	{
		Weapon->StopAim();
	}
}

FRotator AGCBaseCharacter::GetAimOffset()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	return AimDirectionLocal.ToOrientationRotator();
}

void AGCBaseCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	CharacterAttributesComponent->JumpTire();
}

bool AGCBaseCharacter::CanJumpInternal_Implementation() const 
{
	return !MovementComponent->IsOutOfStamina() && Super::CanJumpInternal_Implementation() && !MovementComponent->IsMantling();
}

void AGCBaseCharacter::OnMantle(const FMantlingSettings& MantleSettings, float StartTime)
{
}

void AGCBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.Add(InteractiveActor);
}

void AGCBaseCharacter::UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.Remove(InteractiveActor);
}

void AGCBaseCharacter::OnDeath()
{
	OnMontageTimerStarted(PlayAnimMontage(OnDeathAnimMontage));
}

void AGCBaseCharacter::TraceLineOfSight()
{
	if (!IsPlayerControlled())
	{
		return;
	}
	FVector ViewLocation;
	FRotator ViewRotation;

	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController == nullptr)
	{
		return;
	}
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	FVector ViewDirection = ViewRotation.Vector();
	FVector TraceEnd = ViewLocation + ViewDirection * LineOfSightDistance;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility);
	{
		if (LineOfSightObject.GetObject() != HitResult.Actor.Get())
		{
			LineOfSightObject = HitResult.Actor.Get();
			FName ActionName;
			if (LineOfSightObject.GetInterface())
			{

				ActionName = LineOfSightObject->GetActionEventName();
			}
			else
			{
				ActionName = NAME_None;
			}
			OnInteractableObjectFound.ExecuteIfBound(ActionName);
		}
	}

}

float AGCBaseCharacter::GetIKOffsetForASocket(const FName& SocketName)
{
	float Result = 0;
	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * (1 - DeltaStep));
	FVector TraceEnd(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * (1 + DeltaStep));
	FHitResult HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	{
		Result = (TraceStart.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * DeltaStep - HitResult.Location.Z) / IKScale;
	}
	return Result;
}