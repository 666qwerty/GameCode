// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlatformInvocator.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInvocatorActivated);

enum class EPlatformSwitchPosition : uint8
{
	Off = 0,
	On = 1
};

UCLASS()
class GAMECODE_API APlatformInvocator : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	APlatformInvocator();
	FOnInvocatorActivated OnInvoctorActivated;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Invoke();

	void Return();
	EPlatformSwitchPosition CurrentPosition = EPlatformSwitchPosition::Off;
	FTimerHandle SwitchTimer;
	UPROPERTY(EditAnywhere)
	float Delay = 1;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
