// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UExplosionComponent();

	UPROPERTY(BlueprintAssignable)
	FOnExplosion OnExplosion;

	UFUNCTION(BlueprintCallable)
	void Explode(AController* Controller);
protected:

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float MaxDamage = 75;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float MinDamage = 30;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float DamageFaloff = 1;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Explosion | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Explosion | Radius")
	float InnerRadius = 300;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Explosion | Radius")
	float OuterRadius = 500;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Explosion | VFX")
	UParticleSystem* ExplosionVFX;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
