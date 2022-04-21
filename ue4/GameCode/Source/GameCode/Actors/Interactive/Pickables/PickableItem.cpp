// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Pickables/PickableItem.h"
#include "GCGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickableItem::APickableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

}

// Called when the game starts or when spawned
void APickableItem::BeginPlay()
{
	Super::BeginPlay();
	UGCGameInstance* GameInstance = StaticCast<UGCGameInstance*>(UGameplayStatics::GetGameInstance(GetWorld()));
	GameInstance->InteractableArray.FindOrAdd(GameInstance->MaxInteractableIndex);
	GameInstance->InteractableArray[GameInstance->MaxInteractableIndex] = this;
	Index = GameInstance->MaxInteractableIndex;
	GameInstance->MaxInteractableIndex++;
}

// Called every frame
void APickableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}