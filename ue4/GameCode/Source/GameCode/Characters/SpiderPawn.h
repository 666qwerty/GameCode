// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCodeBasePawn.h"
#include "SpiderPawn.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API ASpiderPawn : public AGameCodeBasePawn
{
    GENERATED_BODY()
public:
    ASpiderPawn();
    virtual void Tick(float DeltaSeconds)override;
    UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKRightFrontFootOffset()const { return IKRightFrontFootOffset; };
    UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKLeftFrontFootOffset() const { return IKLeftFrontFootOffset; };
    UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKRightRearFootOffset() const { return IKRightRearFootOffset; };
    UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetIKLeftRearFootOffset() const { return IKLeftRearFootOffset; };
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName RightFrontFootSocketName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName LeftFrontFootSocketName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName RightRearFootSocketName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName LeftRearFootSocketName;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InterpSpeed = 20;
private:
    float IKRightFrontFootOffset;
    float IKLeftFrontFootOffset;
    float IKRightRearFootOffset;
    float IKLeftRearFootOffset;
    float IKTraceDistance = 0;
    float IKScale = 1;

    float GetIKOffsetForASocket(const FName& SocketName);
};
