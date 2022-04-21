// Fill out your copyright notice in the Description page of Project Settings.


#include "LedgeDetectionComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../../GameCodeTypes.h"
#include "DrawDebugHelpers.h"
#include "../../Utils/GCTraceUtils.h"
#include "../GCBaseCharacter.h"
#include "SubSystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
ULedgeDetectionComponent::ULedgeDetectionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULedgeDetectionComponent::BeginPlay()
{
	Super::BeginPlay();
	
    checkf(GetOwner()->IsA<ACharacter>(), TEXT("ULedgeDetectionComponent has wrong owner"));
    CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
	// ...
	
}

bool ULedgeDetectionComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool IsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryLedgeDetection) && ENABLE_DRAW_DEBUG;
	float BottomZOffset = 2;
	
	ACharacter* DefaultCharacter = CachedCharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	float CapsuleRadius = DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float CapsuleHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleHalfHeight - BottomZOffset) * FVector::UpVector;
	float ForwardCheckCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) / 2;

	FCollisionShape ForwardCollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, ForwardCheckCapsuleHalfHeight);
	FVector ForwardStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

	FHitResult ForwardCheckHitResult;
	float DrawTime=2;

	if (!GCTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, CapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, IsDebugEnabled, DrawTime, FColor::Black, FColor::Red))
	{
		return false;
	}
	
	FCollisionShape DownwardCollisionShape = FCollisionShape::MakeSphere(CapsuleRadius);
	
	float DownwardCheckDepthOffset = 10;
	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + CapsuleRadius;
	FVector DownwardEndLocation = DownwardStartLocation;
	DownwardEndLocation.Z = CharacterBottom.Z;
	FHitResult DownwardCheckHitResult;
	
	if (!GCTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, CapsuleRadius, ECC_Climbing, QueryParams, IsDebugEnabled, DrawTime, FColor::Black, FColor::Red))
	{
		return false;
	}

	FCollisionShape OverlapCollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (CapsuleHalfHeight + BottomZOffset) * FVector::UpVector;
	if (GCTraceUtils::OverlapCapsuleAnyByProfile(GetWorld(), OverlapLocation, CapsuleRadius, CapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, IsDebugEnabled, DrawTime, FColor::Black, FColor::Red))
	{
		return false;
	}
	
	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1, -1, 0)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;
	LedgeDescription.Component = DownwardCheckHitResult.Component;
	return true;
}


// Called every frame
void ULedgeDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

