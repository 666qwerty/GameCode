// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ASpiderPawn::ASpiderPawn()
{
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SpiderMesh"));
    SkeletalMeshComponent->SetupAttachment(RootComponent);
    IKScale = GetActorScale3D().Z;
    IKTraceDistance = CollisionSphereRadius*IKScale;
}

void ASpiderPawn::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    IKRightFrontFootOffset = FMath::FInterpTo(IKRightFrontFootOffset, GetIKOffsetForASocket(RightFrontFootSocketName), DeltaSeconds, InterpSpeed);
    IKLeftFrontFootOffset = FMath::FInterpTo(IKLeftFrontFootOffset, GetIKOffsetForASocket(LeftFrontFootSocketName), DeltaSeconds, InterpSpeed);
    IKRightRearFootOffset = FMath::FInterpTo(IKRightRearFootOffset, GetIKOffsetForASocket(RightRearFootSocketName), DeltaSeconds, InterpSpeed);
    IKLeftRearFootOffset = FMath::FInterpTo(IKLeftRearFootOffset, GetIKOffsetForASocket(LeftRearFootSocketName), DeltaSeconds, InterpSpeed);
}

float ASpiderPawn::GetIKOffsetForASocket(const FName& SocketName)
{
    float Result = 0;
    FVector SocketLocation = SkeletalMeshComponent->GetSocketLocation(SocketName);
    FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
    FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector*2;
    FHitResult HitResult;
    ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
    if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(),EDrawDebugTrace::ForOneFrame,HitResult, true))
    {
	Result = (TraceStart.Z - IKTraceDistance - HitResult.Location.Z) / IKScale;
    }
    return Result;
}
