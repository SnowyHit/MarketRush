// Fill out your copyright notice in the Description page of Project Settings.


#include "MarketCart.h"

#include "CartMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
AMarketCart::AMarketCart()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Enable physics
	
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Root"));
	RootComponent = CollisionComp;
	CharacterVisual = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterVisual"));
	CharacterVisual->SetupAttachment(RootComponent);
	FRWheel = CreateDefaultSubobject<USphereComponent>(TEXT("FR Wheel"));
	FLWheel = CreateDefaultSubobject<USphereComponent>(TEXT("FL Wheel"));
	BRWheel = CreateDefaultSubobject<USphereComponent>(TEXT("BR Wheel"));
	BLWheel = CreateDefaultSubobject<USphereComponent>(TEXT("BL Wheel"));
	FRWheel->SetupAttachment(RootComponent);
	FLWheel->SetupAttachment(RootComponent);
	BRWheel->SetupAttachment(RootComponent);
	BLWheel->SetupAttachment(RootComponent);
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; //

}
// Called when the game starts or when spawned
void AMarketCart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMarketCart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMarketCart::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMarketCart::Boost(const FInputActionValue& Value)
{
	if (UCartMovementComponent* CartMovement = Cast<UCartMovementComponent>(GetMovementComponent()))
	{
		CartMovement->ServerStartBoost(false);
	}
}
void AMarketCart::Jump(const FInputActionValue& Value)
{
	if (UCartMovementComponent* CartMovement = Cast<UCartMovementComponent>(GetMovementComponent()))
	{
		CartMovement->ServerRaiseFrontWheels();
	}
}
void AMarketCart::ReverseBoost(const FInputActionValue& Value)
{
	if (UCartMovementComponent* CartMovement = Cast<UCartMovementComponent>(GetMovementComponent()))
	{
		CartMovement->ServerStartBoost(true);
	}
}
void AMarketCart::StartSlowDown(const FInputActionValue& Value)
{
	if (UCartMovementComponent* CartMovement = Cast<UCartMovementComponent>(GetMovementComponent()))
	{
		CartMovement->ServerSlowDown(true);
	}
}
void AMarketCart::EndSlowDown(const FInputActionValue& Value)
{
	if (UCartMovementComponent* CartMovement = Cast<UCartMovementComponent>(GetMovementComponent()))
	{
		CartMovement->ServerSlowDown(false);
	}
}

// Called to bind functionality to input
void AMarketCart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMarketCart::Move);
		EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Triggered, this, &AMarketCart::Boost);
		EnhancedInputComponent->BindAction(ReverseBoostAction, ETriggerEvent::Triggered, this, &AMarketCart::ReverseBoost);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMarketCart::Jump);
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Triggered, this, &AMarketCart::StartSlowDown);
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Completed, this, &AMarketCart::EndSlowDown);
	}
}


