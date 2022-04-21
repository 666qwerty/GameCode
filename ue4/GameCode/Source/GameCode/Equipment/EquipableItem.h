// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameCodeTypes.h"
#include "EquipableItem.generated.h"

USTRUCT(BlueprintType)
struct FShotInfo
{
	GENERATED_BODY()

	FShotInfo() : Location_Mul_10(FVector_NetQuantize100::ZeroVector), Direction(FVector_NetQuantizeNormal::ZeroVector) {};
	FShotInfo(FVector Location, FVector Direction) : Location_Mul_10(Location * 10), Direction(Direction){};
	FShotInfo(FVector Location, FVector Direction, uint8 Index) : Location_Mul_10(Location * 10), Direction(Direction), Index(Index) {};
	FShotInfo(FVector Location, FVector Direction, uint8 Index, float DamagePercent, float Speed) : Location_Mul_10(Location * 10), Direction(Direction), Index(Index), DamagePercent(DamagePercent), Speed(Speed){};

	UPROPERTY()
	FVector_NetQuantize100 Location_Mul_10;
	UPROPERTY()
	FVector_NetQuantizeNormal Direction;
	UPROPERTY()
	uint8 Index;
	UPROPERTY()
	float DamagePercent;
	UPROPERTY()
	float Speed;

	FVector GetLocation()const { return Location_Mul_10 * 0.1; }
	FVector GetDirection()const { return Direction; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChargeChanged, float, float)
class UAnimMontage;
class AGCBaseCharacter;
UCLASS(Abstract, NotBlueprintable)
class GAMECODE_API AEquipableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquipableItem();
	EEquipableItemType GetItemType() { return EquipableItemType; }
	FName GetUnequippedSocketName() { return UnequippedSocketName; }
	FName GetEquippedSocketName() { return EquippedSocketName; }
	UAnimMontage* GetCharacterEquipAnimMontage() { return CharacterEquipAnimMontage; }
	virtual void Equip() { OnEquipmentStateChanged.Broadcast(true); }
	virtual void UnEquip() { OnEquipmentStateChanged.Broadcast(false); }
	virtual void SetOwner(AActor* NewOwner) override;
	virtual EReticleType GetReticleType() const {return ReticleType;}
	FName GetDataTableId()const { return DataTableId; }
	bool IsSlotCompatible(EEquipmentSlots Slot);
	mutable FOnChargeChanged OnChargeChanged;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEquipableItemType EquipableItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UnequippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* CharacterEquipAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EReticleType ReticleType = EReticleType::None;

	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName DataTableId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<EEquipmentSlots> CompatableEquipmentSlots;
public:	 
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	AGCBaseCharacter* GetCharacterOwner() {return CachedCharacterOwner.Get();}

private:
	TWeakObjectPtr<AGCBaseCharacter> CachedCharacterOwner;

};
