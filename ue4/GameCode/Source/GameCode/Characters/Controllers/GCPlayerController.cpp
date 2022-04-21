// Fill out your copyright notice in the Description page of Project Settings.


#include "GCPlayerController.h"
#include "Characters/GCBaseCharacter.h"
#include "Characters/Components/GCBaseCharacterMovementComponent.h"
#include "Characters/Components/CharacterEquipmentComponent.h"
#include "Characters/Components/CharacterAttributesComponent.h"
#include "UI/PlayerHUDWidget.h"
#include "UI/AmmoWidget.h"
#include "UI/ReticleWidget.h"
#include "UI/WidgetCharacterAttributes.h"
#include "Blueprint/UserWidget.h"
#include "GameCodeTypes.h"
#include "GameFramework/PlayerInput.h"

void AGCPlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);
    CachedBaseCharacter = Cast<AGCBaseCharacter>(InPawn);
	if (IsLocalController() && CachedBaseCharacter.IsValid())
	{
		CreateWidgets();
		CachedBaseCharacter->OnInteractableObjectFound.BindUObject(this, &AGCPlayerController::OnInteractableObjectFound);
	}
}


void AGCPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    InputComponent->BindAxis("MoveForward", this, &AGCPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AGCPlayerController::MoveRight);
    InputComponent->BindAxis("SwimForward", this, &AGCPlayerController::SwimForward);
    InputComponent->BindAxis("SwimRight", this, &AGCPlayerController::SwimRight);
    InputComponent->BindAxis("SwimUp", this, &AGCPlayerController::SwimUp);
    InputComponent->BindAxis("Turn", this, &AGCPlayerController::Turn);
    InputComponent->BindAxis("LookUp", this, &AGCPlayerController::LookUp);
    InputComponent->BindAxis("ClimbLadderUp", this, &AGCPlayerController::ClimbLadderUp);
	InputComponent->BindAction("Slide", IE_Pressed, this, &AGCPlayerController::StartSlide);
	InputComponent->BindAction("Slide", IE_Released, this, &AGCPlayerController::StopSlide);
    InputComponent->BindAction("Crouch", IE_Pressed, this, &AGCPlayerController::ChangeCrouchState);
    InputComponent->BindAction("Prone", IE_Pressed, this, &AGCPlayerController::ChangeProneState);
    InputComponent->BindAction("InteractWithLadder", IE_Pressed, this, &AGCPlayerController::InteractWithLadder);
    InputComponent->BindAction("InteractWithZipline", IE_Pressed, this, &AGCPlayerController::InteractWithZipline);
    InputComponent->BindAction("Mantle", IE_Pressed, this, &AGCPlayerController::Mantle);
	InputComponent->BindAction("Jump", IE_Pressed, this, &AGCPlayerController::Jump);
	InputComponent->BindAction("Fire", IE_Pressed, this, &AGCPlayerController::StartFir);
	InputComponent->BindAction("Fire", IE_Released, this, &AGCPlayerController::StopFir);
	InputComponent->BindAction("Aim", IE_Pressed, this, &AGCPlayerController::StartAim);
	InputComponent->BindAction("Aim", IE_Released, this, &AGCPlayerController::StopAim);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AGCPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AGCPlayerController::StopSprint);
	InputComponent->BindAction("Reload", IE_Pressed, this, &AGCPlayerController::Reload);
	InputComponent->BindAction("NextItem", IE_Pressed, this, &AGCPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", IE_Pressed, this, &AGCPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", IE_Pressed, this, &AGCPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("ChangeWeaponMode", IE_Pressed, this, &AGCPlayerController::ChangeWeaponMode);
	InputComponent->BindAction("PrimaryMeleeAttack", IE_Pressed, this, &AGCPlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", IE_Pressed, this, &AGCPlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", IE_Pressed, this, &AGCPlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction(ActionInteract, IE_Pressed, this, &AGCPlayerController::Interact);
	InputComponent->BindAction("UseInventory", IE_Pressed, this, &AGCPlayerController::UseInventory);
	InputComponent->BindAction("ConfirmWeaponWheelSelection", IE_Pressed, this, &AGCPlayerController::ConfirmWeaponWheelSelection);
	FInputActionBinding& ToggleMenuBinding = InputComponent->BindAction("ToggleMainMenu", IE_Pressed, this, &AGCPlayerController::ToggleMainMenu);
	ToggleMenuBinding.bExecuteWhenPaused = true;
}

void AGCPlayerController::ConfirmWeaponWheelSelection()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ConfirmWeaponSelection();
	}
}

void AGCPlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void AGCPlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void AGCPlayerController::ChangeWeaponMode()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->GetGCEquipmentComponent()->ChangeWeaponMode();
	}
}

void AGCPlayerController::ToggleMainMenu()
{
	if (!IsValid(MainMenuWidget) || !IsValid(PlayerHUDWidget))
	{
		return;
	}
	if (MainMenuWidget->IsVisible())
	{
		MainMenuWidget->RemoveFromParent();
		PlayerHUDWidget->AddToViewport();
		SetInputMode(FInputModeGameOnly{});
		SetPause(false);
		bShowMouseCursor = false;
	}
	else
	{
		MainMenuWidget->AddToViewport();
		PlayerHUDWidget->RemoveFromParent();
		SetInputMode(FInputModeGameAndUI{});
		SetPause(true);
		bShowMouseCursor = true;
	}
}

void AGCPlayerController::Interact()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}

void AGCPlayerController::UseInventory()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->UseInventory(this);
	}
}

void AGCPlayerController::OnInteractableObjectFound(FName ActionName)
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}

	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0;
	if (HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName();
		PlayerHUDWidget->SetHighlightInteractableActionText(ActionKey);
	}
	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);
}

void AGCPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AGCPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AGCPlayerController::MoveForward(float Value)
{
	
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
		CachedBaseCharacter->SetForwardAxis(Value);
	}
}

void AGCPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
		CachedBaseCharacter->SetRightAxis(Value);
	}
}

void AGCPlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AGCPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AGCPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AGCPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AGCPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AGCPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AGCPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AGCPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AGCPlayerController::StartAim()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAim();
	}
}

void AGCPlayerController::StopAim()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAim();
	}
}

void AGCPlayerController::StartFir()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AGCPlayerController::StopFir()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AGCPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AGCPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AGCPlayerController::CreateWidgets()
{
	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
	}
	else
	{
		return;
	}
	if (!IsValid(MainMenuWidget))
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
	}

	if (IsValid(PlayerHUDWidget))
	{
		SetInputMode(FInputModeGameOnly{});
		bShowMouseCursor = false;
		PlayerHUDWidget->AddToViewport();
		if (CachedBaseCharacter.IsValid())
		{
			UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
			if (IsValid(ReticleWidget))
			{
				CachedBaseCharacter->OnAimStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
				CachedBaseCharacter->GetGCEquipmentComponent()->OnEquippedItemChanged.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
			}
			UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
			if (IsValid(AmmoWidget))
			{
				CachedBaseCharacter->GetGCEquipmentComponent()->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
			}
			/*UWidgetCharacterAttributes* HealthWidget = PlayerHUDWidget->GetHealthWidget();
			if (IsValid(HealthWidget))
			{
				HealthWidget->SetColor(FLinearColor::Red);
				CachedBaseCharacter->GetAttributesComponent()->OnHealthChangedEvent.AddUFunction(HealthWidget, FName("UpdateAmount"));
			}
			UWidgetCharacterAttributes* StaminaWidget = PlayerHUDWidget->GetStaminaWidget();
			if (IsValid(StaminaWidget))
			{
				StaminaWidget->SetColor(FLinearColor::Yellow);
				CachedBaseCharacter->GetAttributesComponent()->OnStaminaChangedEvent.AddUFunction(StaminaWidget, FName("UpdateAmount"));
			}
			UWidgetCharacterAttributes* OxygenWidget = PlayerHUDWidget->GetOxygenWidget();
			if (IsValid(OxygenWidget))
			{
				OxygenWidget->SetColor(FLinearColor::Blue);
				CachedBaseCharacter->GetAttributesComponent()->OnOxygenChangedEvent.AddUFunction(OxygenWidget, FName("UpdateAmount"));
			}*/
		}
	}
}

void AGCPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AGCPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AGCPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AGCPlayerController::StartSlide()
{
	if (CachedBaseCharacter.IsValid() && CachedBaseCharacter->GetGCMovementComponent())
	{
		CachedBaseCharacter->GetGCMovementComponent()->StartSlide();
	}
}

void AGCPlayerController::StopSlide()
{
	if (CachedBaseCharacter.IsValid() && CachedBaseCharacter->GetGCMovementComponent())
	{
		CachedBaseCharacter->GetGCMovementComponent()->StopSlide();
	}
}

void AGCPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void AGCPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AGCPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}
