// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GCPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API AGCPlayerController : public APlayerController
{
    GENERATED_BODY()
	
public:
    virtual void SetPawn(APawn* InPawn) override;
    bool GetIgnoreCameraPitch() const { return bIgnoreCameraPitch; }
    void SetIgnoreCameraPitch(bool bCameraPitch) { bIgnoreCameraPitch = bCameraPitch; }

protected:
    virtual void SetupInputComponent() override;
    //virtual void CreateWidgets() override;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<class UUserWidget> MainMenuWidgetClass;
private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void SwimForward(float Value);
    void SwimRight(float Value);
    void SwimUp(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void ClimbLadderUp(float Value);
    void ChangeCrouchState();
    void ChangeProneState();
    void Jump();
    void StartAim();
    void StopAim();
    void StopFir();
    void StartFir();
    void StartSprint();
    void StopSprint();
    void StartSlide();
    void StopSlide();
    void Mantle();
    void Reload();
    void PrimaryMeleeAttack();
    void SecondaryMeleeAttack();
    void NextItem();
    void PreviousItem();
    void InteractWithLadder();
    void InteractWithZipline();
    void EquipPrimaryItem();
    void ChangeWeaponMode();
    void ToggleMainMenu();
    void Interact();
    void UseInventory();
    void ConfirmWeaponWheelSelection();
    void OnInteractableObjectFound(FName ActionName);

    TSoftObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
    bool bIgnoreCameraPitch = false;
    void CreateWidgets();
    class UPlayerHUDWidget* PlayerHUDWidget = nullptr;
    class UUserWidget* MainMenuWidget = nullptr;

};
