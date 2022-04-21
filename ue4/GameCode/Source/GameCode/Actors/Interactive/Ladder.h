// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Ladder.generated.h"

class UStaticMeshComponent;
class UAnimMontage;
class UBoxComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class GAMECODE_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()
public:
	ALadder();

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transfrom) override; 
	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult) override;

	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	float GetHeight() const {return LadderHeight;} 

	bool GetIsOnTop() const{return bIsOnTop;}
	
	UAnimMontage* GetAttachFromTopAnimMontage() const{return AttachFromTopAnimMontage;}
	FVector GetAttachFromTopAnimMontageStartingLocation() const;

protected:
	UBoxComponent* GetLadderInteractionBox() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float LadderHeight = 100;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float LadderWidth = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float LadderInterval = 25;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float LadderBottomStep = 25;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	FVector AttachFromTopAnimMontageInitialOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	class UStaticMeshComponent* RightRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	class UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	class UInstancedStaticMeshComponent* StepsMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	class UBoxComponent* TopInteractionVolume;

private:
	bool bIsOnTop = false;

};
