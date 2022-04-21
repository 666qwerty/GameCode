// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaEventSignature, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAmountChanged, float, bool)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterAttributesComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;
	FOutOfStaminaEventSignature OutOfStaminaEvent;
	FOnAmountChanged OnHealthChangedEvent;
	FOnAmountChanged OnStaminaChangedEvent;
	FOnAmountChanged OnOxygenChangedEvent;

	bool IsAlive() { return Health > 0; }
	void Tire(float StaminaConsumed);
	void JumpTire() { Tire(JumpStaminaCost); };
	float GetHealthPercent() const { return Health / MaxHealth; };
	void OnHealthChanged();

	void AddHealth(float Value);
	void AddStamina(float Value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float MaxStamina = 100;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float StaminaRestoreVelocity = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float SprintStaminaConsumptionVelocity = 20;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float JumpStaminaCost = 40; 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen")
	float MaxOxygen = 50.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen")
	float OxygenRestoreVelocity = 15.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen")
	float SwimOxygenConsumptionVelocity = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen")
	float NoOxygenDamage = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen")
	float NoOxygenInterval = 2.0f;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health = 0;
	float Stamina = 0;
	float Oxygen = 0;
	float NoOxygenTimer = 0.0f;
	void DebugDrawAttributes();
	UFUNCTION()
	void OnRep_Health();
	void UpdateStamina(float DeltaTime);
	void UpdateOxygen(float DeltaTime);

	UFUNCTION()
	void OnTakeAnyDamage(AActor* Actor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:
	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacterOwner;
};
