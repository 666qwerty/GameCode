// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeDetectionComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Location;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator Rotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LedgeNormal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TWeakObjectPtr<class UPrimitiveComponent> Component;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API ULedgeDetectionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULedgeDetectionComponent();

	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinimumLedgeHeight = 40;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaximumLedgeHeight = 200;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ForwardCheckDistance = 100;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;

		
};
