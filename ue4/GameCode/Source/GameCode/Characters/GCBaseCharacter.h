// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameCodeTypes.h"
#include "GenericTeamAgentInterface.h"
#include "Curves/CurveVector.h"
#include "UObject/ScriptInterface.h"
#include "GCBaseCharacter.generated.h"

class UCharacterInventoryComponent;
class UGCBaseCharacterMovementComponent;
class AInteractiveActor;
class IInteractable;
class UWidgetComponent;
enum class EWallRunSide : uint8;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimStateChanged, bool)

DECLARE_DELEGATE_OneParam(FOnInteractableObjectFound, FName)

USTRUCT(BlueprintType)
struct FMantlingSettings
{
    GENERATED_BODY();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    class UAnimMontage* MantlingMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    class UAnimMontage* FPMantlingMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    class UCurveVector* MantlingCurve;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AnimationCorrectionXY = 65;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float AnimationCorrectionZ = 200;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxHeight = 200;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinHeight = 100;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxHeightStartTime = 0;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinHeightStartTime = 0.5;
};

UCLASS(Abstract, NotBlueprintable)
class GAMECODE_API AGCBaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AGCBaseCharacter(const FObjectInitializer& ObjectInitializer);
    // Called every frame
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void OnJumped_Implementation() override;
    
    virtual void MoveForward(float Value) {};
    virtual void MoveRight(float Value) {};
    virtual void SwimForward(float Value) {};
    virtual void SwimRight(float Value) {};
    virtual void SwimUp(float Value) {};
    virtual void Turn(float Value) {};
    virtual void LookUp(float Value) {};
    virtual void ClimbLadderUp(float Value);
    virtual void Jump();
    virtual void StartFire();
    virtual void StopFire();
    virtual void StartAim();
    virtual void StopAim();
    FRotator GetAimOffset();
    float GetAimingMovementSpeed() const {return CurrentAimingMovementSpeed;}
    bool IsAim() const { return bIsAim; }
    void SetIsAim(bool bIsAim_In) { bIsAim = bIsAim_In;}
    virtual void ChangeCrouchState();
    virtual void ChangeProneState();
    virtual void StartSprint();
    virtual void StopSprint();
    UFUNCTION(BlueprintCallable)
    void Mantle();
    void Reload();
    void NextItem();
    void PreviousItem();
    void EquipPrimaryItem();
    void PrimaryMeleeAttack();
    void SecondaryMeleeAttack();
    void Interact();
    bool PickupItem(TWeakObjectPtr<class UInventoryItem> ItemToPickup);
    bool PickupAmmo(TWeakObjectPtr<class UAmmoInventoryItem> ItemToPickup);
    void UseInventory(APlayerController* PlayerController);
    void AddHealth(float Value);
    void AddStamina(float Value);

    FOnInteractableObjectFound OnInteractableObjectFound;
    virtual void InteractWithLadder();
    virtual void InteractWithZipline();
    const class ALadder* GetAvailableLadder() const;
    const class AZipline* GetAvailableZipline() const;
    bool AreRequiredKeysDown(EWallRunSide Side) const {return true;}
    virtual void SetForwardAxis(float Value) {}
    virtual void SetRightAxis(float Value) {}
    virtual void PossessedBy(AController* NewController) override;

    virtual void Landed(const FHitResult& Hit) override;
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnStartAim();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnStopAim();
    FOnAimStateChanged OnAimStateChanged;
    
    UFUNCTION(BlueprintCallable)
    FORCEINLINE float GetIKRightFootOffset() const { return IKRightFootOffset; };
    UFUNCTION(BlueprintCallable)
    FORCEINLINE float GetIKLeftFootOffset() const { return IKLeftFootOffset; };
    UFUNCTION(BlueprintCallable)
    FORCEINLINE float GetIKPelvisOffset() const { return IKPelvisOffset; };
    FORCEINLINE UGCBaseCharacterMovementComponent* GetGCMovementComponent() { return MovementComponent; };
    class UCharacterEquipmentComponent* GetGCEquipmentComponent() { return CharacterEquipmentComponent; };
    
    virtual void OnProned(float HalfHeightAdjust, float ScaledHalfHeightAdjust) {};
    void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
    void UnRegisterInteractiveActor(AInteractiveActor* InteractiveActor);

    void PlaySlideAnimation();

    bool IsUnderWater() const;

    class UCharacterAttributesComponent* GetAttributesComponent(){ return CharacterAttributesComponent; };

    /**IGenericTeamAgentInterface*/
    virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId((uint8)Team); }
    /**~IGenericTeamAgentInterface*/

    UPROPERTY(ReplicatedUsing = OnRep_IsMantling)
    bool bIsMantling;

    UFUNCTION()
    void OnRep_IsMantling(bool bWasMantling);

    void ConfirmWeaponSelection();
protected:

    UFUNCTION(BlueprintNativeEvent, Category = "CharacterMovement")
    void OnStartSprint();
    virtual void OnStartSprint_Implementation();
    UFUNCTION(BlueprintNativeEvent, Category = "CharacterMovement")
    void OnStopSprint();
    virtual void OnStopSprint_Implementation();

    virtual void OnStartAim_Implementation();
    virtual void OnStopAim_Implementation();

    virtual bool CanSprint();
    void TryChangeSprint(float DeltaTime);
    
    virtual bool CanMantle() const;
    virtual bool CanJumpInternal_Implementation() const override;

    virtual void OnMantle(const FMantlingSettings& MantleSettings, float StartTime);
    virtual void OnMontageTimerStarted(float Duration);
    virtual void OnMontageTimerElapsed();

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    UWidgetComponent* HealthbarProgressComponent;

    void InitializeHealthProgress();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName RightFootSocketName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName LeftFootSocketName;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InterpSpeed = 20;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float DeltaStep = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class ULedgeDetectionComponent* LedgeDetectionComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FMantlingSettings HeightMantlingSettings;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FMantlingSettings LowMantlingSettings;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float LowMantleMaxHeight = 125;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide Parameters")
    UAnimMontage* SlideAnimMontage;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HardLanding Parameters")
    UAnimMontage* HardLandingMontage;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HardLanding Parameters")
    float HardLandingSpeed = 1000.0f;
    UGCBaseCharacterMovementComponent* MovementComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Equipment")
    class UCharacterEquipmentComponent* CharacterEquipmentComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Attributes")
    class UCharacterAttributesComponent* CharacterAttributesComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
    class UCurveFloat* FallDamageCurve;

    virtual void OnDeath();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animations")
    UAnimMontage* OnDeathAnimMontage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
    ETeams Team = ETeams::Enemy;

    bool bIsAim = false;
        
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
    float LineOfSightDistance = 500;

    void TraceLineOfSight();

    UFUNCTION(Server, Reliable)
    void Server_Interact(int32 Index);

    UPROPERTY()
    TScriptInterface<IInteractable> LineOfSightObject;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
    UCharacterInventoryComponent* CharacterInventoryComponent;

private:
    float GetIKOffsetForASocket(const FName& SocketName);
    void UpdateIK(float DeltaTime);
    const FMantlingSettings& GetMantlingSettings(float LedgeHeight);

    TSet<AInteractiveActor*> AvailableInteractiveActors;
    float IKRightFootOffset;
    float IKLeftFootOffset;
    float IKPelvisOffset;
    float IKTraceDistance = 0;
    float IKScale = 1;
    bool bIsSprintRequested = false;
    float CurrentAimingMovementSpeed;
    FTimerHandle MontageTimer; 
};