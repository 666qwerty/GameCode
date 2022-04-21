// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCodeBasePawn.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/GCBasePawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../GameCode.h"

// Sets default values
AGameCodeBasePawn::AGameCodeBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	qMovementComponent = CreateDefaultSubobject<UGCBasePawnMovementComponent, UGCBasePawnMovementComponent>(TEXT("MovementComponent"));
	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("MovementComponent"));
	qMovementComponent->SetUpdatedComponent(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = 1;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif
}

// Called when the game starts or when spawned
void AGameCodeBasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager * CameraManager= UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CurrentActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));
	
}

// Called every frame
void AGameCodeBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameCodeBasePawn::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(CurrentActor->GetActorForwardVector(), Value);
	}
	InputForward = Value;
}


void AGameCodeBasePawn::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(CurrentActor->GetActorRightVector(), Value);
	}
	InputRight = Value;
}

void AGameCodeBasePawn::JumpStart()
{
	StaticCast<UGCBasePawnMovementComponent*>(qMovementComponent)->JumpStart();
}


// Called to bind functionality to input
void AGameCodeBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGameCodeBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGameCodeBasePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, qMovementComponent, &UGCBasePawnMovementComponent::JumpStart);


}

void AGameCodeBasePawn::OnBlendComplete()
{
	CurrentActor = GetController()->GetViewTarget();
	UE_LOG(LogCamera, Verbose, TEXT("OnBlendComplete Current view: %s"), *CurrentActor->GetName());
}

