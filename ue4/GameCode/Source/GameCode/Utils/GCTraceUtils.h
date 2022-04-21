#pragma once

namespace GCTraceUtils
{
	bool SweepCapsuleSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, bool bDrawDebug = false, float DrawTime = -1, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);
	bool SweepSphereSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float Radius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, bool bDrawDebug = false, float DrawTime = -1, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam);
	bool OverlapCapsuleAnyByProfile(const UWorld* World, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, bool bDrawDebug = false, float DrawTime = -1, FColor TraceColor = FColor::Transparent, FColor HitColor = FColor::Red);
}