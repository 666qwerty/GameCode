// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICharacterSpawner.generated.h"

class AGCAICharacter;
class IInteractable;
UCLASS()
class GAMECODE_API AAICharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAICharacterSpawner();

	UFUNCTION()
	void SpawnAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGCAICharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSpawnOnStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDoOnce;

	//An actor implementing IInteractableInterface
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* SpawnTriggerActor;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	void UnSubscribeFromTrigger();

	UPROPERTY()
	TScriptInterface<IInteractable> SpawnTrigger;

	FDelegateHandle TriggerHandle;
	bool bCanSpawn = true;
};
