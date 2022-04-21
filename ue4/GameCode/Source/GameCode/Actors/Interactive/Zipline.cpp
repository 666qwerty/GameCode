	// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include "Components/CapsuleComponent.h"
#include "../../GameCodeTypes.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));
	BottomRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomRail"));
	BottomRailMeshComponent->SetupAttachment(RootComponent);
	TopRailMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopRail"));
	TopRailMeshComponent->SetupAttachment(RootComponent);
	WireComponent2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wire2"));
	WireComponent2->SetupAttachment(RootComponent);
	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();
	FVector TopTopRail = TopRailMeshComponent->GetComponentLocation();
	FVector TopBottomRail = BottomRailMeshComponent->GetComponentLocation();
	TopTopRail.Z += TopRailHeight / 2;
	TopBottomRail.Z += BottomRailHeight / 2;
	WireDirection = (TopTopRail - TopBottomRail).GetSafeNormal();
}

bool AZipline::IsInverted() const
{
	return WireDirection.Z < 0;
}

FVector AZipline::GetWireDirection() const
{
	return WireDirection;
}

void AZipline::OnConstruction(const FTransform& Transfrom)
{
	BottomRailMeshComponent->SetRelativeLocation(FVector(0, 0, BottomRailHeight / 2));
	TopRailMeshComponent->SetRelativeLocation(FVector(0, Distance, DeltaHeight + TopRailHeight / 2));
	float TopDeltaHeight = DeltaHeight + TopRailHeight - BottomRailHeight;
	
	UStaticMesh* BottomRailMesh = BottomRailMeshComponent->GetStaticMesh();
	if (IsValid(BottomRailMesh))
	{
		float MeshHeight = BottomRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			BottomRailMeshComponent->SetRelativeScale3D(FVector(1, 1, BottomRailHeight / MeshHeight));
		}
	}
	UStaticMesh* TopRailMesh = TopRailMeshComponent->GetStaticMesh();
	if (IsValid(TopRailMesh))
	{
		float MeshHeight = TopRailMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshHeight))
		{
			TopRailMeshComponent->SetRelativeScale3D(FVector(1, 1, TopRailHeight / MeshHeight));
		}
	}

	float Angle = FMath::Atan(TopDeltaHeight / Distance);
	
	UStaticMesh* WireMesh = WireComponent2->GetStaticMesh();
	if (IsValid(WireMesh))
	{
		float MeshLength = WireMesh->GetBoundingBox().GetSize().Z;
		if (!FMath::IsNearlyZero(MeshLength))
		{
			WireComponent2->SetRelativeLocation(FVector(0, Distance / 2, BottomRailHeight + TopDeltaHeight / 2));
			WireComponent2->SetRelativeScale3D(FVector(1, 1, Distance / cos(Angle) / MeshLength));
			FRotator Rot =  FRotationMatrix::MakeFromZ(FVector(0, Distance, TopDeltaHeight)).Rotator();
			
			WireComponent2->SetRelativeRotation(Rot);
			GetZiplineInteractionCapsule()->SetRelativeLocation(FVector(0, Distance / 2, BottomRailHeight + TopDeltaHeight / 2));
			GetZiplineInteractionCapsule()->SetCapsuleHalfHeight(Distance / cos(Angle) / 2);
			GetZiplineInteractionCapsule()->SetRelativeRotation(Rot);
		}
	}
}

void AZipline::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AZipline::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

UCapsuleComponent* AZipline::GetZiplineInteractionCapsule() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}

float AZipline::GetHeightFromDistance(float InDistance) const
{
	float Ratio = InDistance / Distance;
	return BottomRailHeight + Ratio * (TopRailHeight + DeltaHeight - BottomRailHeight);
}