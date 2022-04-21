#include "GCTraceUtils.h"
#include "DrawDebugHelpers.h"

bool GCTraceUtils::SweepCapsuleSingleByChannel(const UWorld* World, FHitResult& OutHit, const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor, const FCollisionResponseParams& ResponseParam)
{
	bool bResult = false;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = World->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollisionShape, Params, ResponseParam);
#if ENABLE_DRAW_DEBUG
	if(bDrawDebug)
	{
		DrawDebugCapsule(World, Start, CapsuleHalfHeight, CapsuleRadius, Rot, TraceColor, false, DrawTime);
		DrawDebugCapsule(World, End, CapsuleHalfHeight, CapsuleRadius, Rot, TraceColor, false, DrawTime);
		DrawDebugLine(World, Start, End, TraceColor, false, 2);
		if (bResult)
		{
			DrawDebugCapsule(World, OutHit.Location, CapsuleHalfHeight, CapsuleRadius, Rot, HitColor, false, DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10, HitColor, false, DrawTime);
		}
	}
#endif
	return bResult;
}

bool GCTraceUtils::SweepSphereSingleByChannel(const UWorld* World, FHitResult& OutHit, const FVector& Start, const FVector& End, float Radius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor, const FCollisionResponseParams& ResponseParam)
{
	bool bResult = false;
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);
	bResult = World->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, CollisionShape, Params, ResponseParam);
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		FVector DebugCapsule = (Start + End) / 2;
		FQuat Rot = FRotationMatrix::MakeFromZ(End - Start).ToQuat();
		DrawDebugCapsule(World, DebugCapsule, (End - Start).Size() / 2, Radius, Rot, TraceColor, false, DrawTime);
		if (bResult)
		{
			//DrawDebugSphere(World, OutHit.Location, Radius, 16, HitColor, false, DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10, HitColor, false, DrawTime);
		}
	}
#endif
	return bResult;
}

bool GCTraceUtils::OverlapCapsuleAnyByProfile(const UWorld* World, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, FName ProfileName, const FCollisionQueryParams& Params, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	bool bResult = false;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	bResult = World->OverlapBlockingTestByProfile(Pos, Rot, ProfileName, CollisionShape, Params);
	//bResult = World->OverlapAnyTestByProfile(Pos, Rot, ProfileName, CollisionShape, Params);
	
#if ENABLE_DRAW_DEBUG
	if (bDrawDebug && bResult)
	{
		DrawDebugCapsule(World, Pos, CapsuleHalfHeight, CapsuleRadius, Rot, bResult ? HitColor : TraceColor, false, DrawTime);
	}
#endif
	return bResult;
}
