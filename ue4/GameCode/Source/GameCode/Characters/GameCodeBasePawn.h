// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameCodeBasePawn.generated.h"

UCLASS()
class GAMECODE_API AGameCodeBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameCodeBasePawn();

	UPROPERTY(VisibleAnywhere)
	class UGCBasePawnMovementComponent* qMovementComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void JumpStart();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BasePawn")
	float CollisionSphereRadius = 50;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BasePawn")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BasePawn")
	class UCameraComponent* CameraComponent;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BasePawn")
	class UArrowComponent* ArrowComponent;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	float GetInputForward() const { return InputForward; }
	UFUNCTION(BlueprintCallable)
	float GetInputRight() const { return InputRight; }

private:
	UFUNCTION()
	void OnBlendComplete();
	AActor* CurrentActor;
	float InputForward=0;
	float InputRight=0;

};
