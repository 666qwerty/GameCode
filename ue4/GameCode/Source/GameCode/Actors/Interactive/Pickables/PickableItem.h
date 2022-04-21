// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "PickableItem.generated.h"

UCLASS(Abstract, NotBlueprintable)
class GAMECODE_API APickableItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickableItem();
	const FName& GetDataTableId() {	return DataTableId;	}
	int32 GetIndex() { return Index; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DataTableId = NAME_None;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	int32 Index;
};
