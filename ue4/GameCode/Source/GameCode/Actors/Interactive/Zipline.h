// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Zipline.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()
public:
	AZipline();

	virtual void OnConstruction(const FTransform& Transfrom) override; 
	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;

	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	float GetHeightFromDistance(float InDistance) const;

	virtual void BeginPlay() override;

	FVector GetWireDirection() const;

	bool IsInverted() const;

protected:
	UCapsuleComponent* GetZiplineInteractionCapsule() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline Parameters")
	float BottomRailHeight = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline Parameters")
	float TopRailHeight = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline Parameters")
	float Distance = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline Parameters")
	float DeltaHeight = 500;
	
	FVector WireDirection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline Parameters")
	class UStaticMeshComponent* TopRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline Parameters")
	class UStaticMeshComponent* BottomRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zipline Parameters")
	class UStaticMeshComponent* WireComponent2;
};
