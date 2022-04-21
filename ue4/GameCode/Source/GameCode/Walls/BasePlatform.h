// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformBehavior : uint8
{
	None = 0,
	OnDemand = 1,
	Loop = 2
};

enum class EPlatformPosition : uint8
{
	Start = 0,
	End = 1,
	Moving = 2
};

UCLASS()
class GAMECODE_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void PlatformTimelineUpdate(float Alpha);
	UFUNCTION(BlueprintCallable)
	void StartMoving();
	UFUNCTION(BlueprintCallable)
	void StartMovingWithDelay(float delay);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* TimelineCurve;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::None;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector StartLocation;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MovementCooldown = 1;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	class APlatformInvocator* PlatformInvocator;

	FTimeline PlatformTimeline;
	FTimerHandle PlatformMovementTimer;
	EPlatformPosition CurrentPosition = EPlatformPosition::Start;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
