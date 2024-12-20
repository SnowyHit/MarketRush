// Fill out your copyright notice in the Description page of Project Settings.


#include "CartMovementComponent.h"
#include "MarketCart.h"


class AMarketCart;
// Sets default values for this component's properties
UCartMovementComponent::UCartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bIsBoosting = false;
	bCanBoost = true;
	MaxVelocity = 1400.0f;
	TurnRate = 800.0f;
	SlowDownFactor = 1.0f;
	BoostSpeed = 750.0f;
	BoostCooldown = 0.75f;
	MaxRoll = 50.0f;
	MaxPitch = 50.0f;
	ToppledDuration =3.0f;
	// Reduce velocity to 50%
	bIsSlowingDown = false;
	CurrentState = ECartState::Idle;
	SetIsReplicated(true);
	// ...
}

void UCartMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	ServerUpdateRepTransformCart(UpdatedComponent->GetComponentLocation(),UpdatedComponent->GetComponentRotation());
}

void UCartMovementComponent::ClientStartBoost_Implementation(bool IsReversed)
{
	StartBoost(IsReversed);
	ServerStartBoost(IsReversed);	
}

void UCartMovementComponent::ClientSlowDown_Implementation(bool Start)
{
	Start ? StartSlowDown() : StopSlowDown();
	ServerSlowDown(Start);
}
void UCartMovementComponent::ClientTurnCart_Implementation(float TurnIntensity)
{
	TurnCart(TurnIntensity);
	ServerTurnCart(TurnIntensity);
}

void UCartMovementComponent::ClientResetCart_Implementation()
{
	ResetCart();
	ServerResetCart();
}

void UCartMovementComponent::ClientRaiseFrontWheels_Implementation()
{
	RaiseFrontWheels();
	ServerRaiseFrontWheels();
}

void UCartMovementComponent::ServerStartBoost_Implementation(bool IsReversed)
{
	StartBoost(IsReversed);
}

void UCartMovementComponent::ServerSlowDown_Implementation(bool Start)
{
	if(Start)
	{
		StartSlowDown();
	}
	else
	{
		StopSlowDown();
	}
}

void UCartMovementComponent::ServerTurnCart_Implementation(float TurnIntensity)
{
	TurnCart(TurnIntensity);
}

void UCartMovementComponent::ServerResetCart_Implementation()
{
	ResetCart();
}
void UCartMovementComponent::ServerRaiseFrontWheels_Implementation()
{
	RaiseFrontWheels();
}
void UCartMovementComponent::ServerUpdateRepTransformCart_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
	ReplicatedRotation = NewRotation;
	ReplicatedLocation = NewLocation;
}
void UCartMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCartMovementComponent, ReplicatedRotation);
	DOREPLIFETIME(UCartMovementComponent, ReplicatedLocation);
}


bool UCartMovementComponent::IsOwnedByLocalPlayer() const
{
	// Check if the owner of this component is controlled by the local player
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	const APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn)
	{
		return false;
	}

	return OwnerPawn->IsLocallyControlled();
}
// Called every frame
void UCartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(IsOwnedByLocalPlayer())
	{
		ServerUpdateRepTransformCart(UpdatedComponent->GetComponentLocation() , UpdatedComponent->GetComponentRotation());
	}
	else
	{
		//This Update gives information to other clients. Set the location of non player controlled player pawns.
		UpdatedComponent->SetWorldLocation(ReplicatedLocation);
		UpdatedComponent->SetWorldRotation(ReplicatedRotation,true);
	}
	bool bIsGrounded = IsGrounded();
	bool bIsUpright = IsCartUpright(0);
	UpdateCartState(bIsGrounded, bIsUpright);
    	
	AnimSpeed = UpdatedComponent->GetComponentVelocity().Size();
}

bool UCartMovementComponent::IsFrontWheelLifted() const
{
	FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
	return CurrentRotation.Pitch > MaxPitch / 2; // Adjust the threshold as needed
}
void UCartMovementComponent::UpdateCartState(bool bIsGrounded, bool bIsUpright)
{
	if(CurrentState == ECartState::Crashed)
	{
		return;
	}
	FVector InputVector = GetPendingInputVector();

	// Check conditions and update state accordingly
	if (!bIsGrounded)
	{
		CurrentState = ECartState::NotGrounded;
	}
	else if (!bIsUpright)
	{
		if (CurrentState != ECartState::Toppled)
		{
			// Transition to Toppled state and start the crash timer
			CurrentState = ECartState::Toppled;
			GetOwner()->GetWorldTimerManager().SetTimer(
				ToppledTimerHandle,
				this,
				&UCartMovementComponent::TransitionToCrashed,
				ToppledDuration,
				false
			);
		}
	}
	else if (bIsSlowingDown)
	{
		CurrentState = ECartState::SlowingDown;
	}
	else if (bIsBoosting)
	{
		CurrentState = ECartState::Boosting;
	}
	else if (!InputVector.IsNearlyZero())
	{
		if (FMath::Abs(InputVector.Y) > 0.1f)
		{
			CurrentState = (InputVector.Y > 0.0f) ? ECartState::TurningRight : ECartState::TurningLeft;
		}
	}
	else if (IsFrontWheelLifted())
	{
		CurrentState = ECartState::Wheelie;
	}
	else
	{
		CurrentState = ECartState::Upright;
	}

	// If the cart recovers from Toppled state, clear the crash timer
	if (CurrentState != ECartState::Toppled && GetOwner()->GetWorldTimerManager().IsTimerActive(ToppledTimerHandle))
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(ToppledTimerHandle);
  
	}

	// Debug output for testing
	//UE_LOG(LogTemp, Log, TEXT("Current Cart State: %s"), *UEnum::GetValueAsString(CurrentState));
}
void UCartMovementComponent::TransitionToCrashed()
{
	if (CurrentState == ECartState::Toppled)
	{
		CurrentState = ECartState::Crashed;
		//UE_LOG(LogTemp, Warning, TEXT("Cart has been in Toppled state for too long. Transitioning to Crashed."));
		ClientResetCart();
	}
}
void UCartMovementComponent::ResetCart()
{
	// Reset position and rotation
	if (AActor* Owner = GetOwner())
	{
		FVector ResetPosition = Owner->GetActorLocation();
		ResetPosition.Z += 100.0f; // Lift slightly to avoid clipping into the ground
		Owner->SetActorLocation(ResetPosition);

		FRotator ResetRotation = FRotator(0.0f, Owner->GetActorRotation().Yaw, 0.0f); // Keep only Yaw rotation
		Owner->SetActorRotation(ResetRotation);

		UE_LOG(LogTemp, Log, TEXT("Cart position and rotation have been reset."));
	}

	// Clear any active timers
	GetOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);

	// Reset state to Upright
	CurrentState = ECartState::Upright;
	UE_LOG(LogTemp, Log, TEXT("Cart state has been reset to Upright."));
}
void UCartMovementComponent::RaiseFrontWheels()
{
	FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
	if(!IsCartUpright(5))
	{
		return;
	}
	if(!IsGrounded())
	{
		return;
	}
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
	{
		// Get the current rotation and calculate the forward vector
		FVector ForwardVector = FRotationMatrix(CurrentRotation).GetScaledAxis(EAxis::Y); // Forward direction

		// Define a raise input
		float RaiseInput = 1.0f; // You can read this value from actual input

		// Calculate the torque to apply based on the forward direction
		FVector TorqueToLift = ForwardVector * (RaiseInput * TurnRate); // Apply torque in the forward direction

		// Get the current velocity of the cart
		FVector CurrentVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();

		// Increase torque if moving slowly
		if (CurrentVelocity.Size() < 100.0f) // Adjust as needed to determine low speed for torque
		{
			TorqueToLift *= 50; // Increase torque when moving slowly to lift
		}

		// Apply the torque to lift the front of the cart
		PrimitiveComponent->AddTorqueInRadians(-TorqueToLift, NAME_None, true);
	}
}
bool UCartMovementComponent::IsCartUpright(float Tolerance) const
{
	FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Allowable roll in degrees

	// Check if the cart is upright (within acceptable limits for pitch and roll)
	return FMath::Abs(CurrentRotation.Pitch) <= MaxPitch - Tolerance && 
		   FMath::Abs(CurrentRotation.Roll) <= MaxRoll - Tolerance;
}
bool UCartMovementComponent::IsGrounded() const
{
	AMarketCart* MarketCart = Cast<AMarketCart>(PawnOwner);
	if (!MarketCart)
	{
		return false; // In case the cast fails, return false
	}

	// Store the hit result to retrieve information if needed
	FHitResult HitResult;
	FVector Start;
	FVector End;

	// Check for the right back wheel
	if (MarketCart->BRWheel)
	{
		Start = MarketCart->BRWheel->GetComponentLocation(); // Get the location of the back right wheel
		End = Start - FVector(0, 0, 1) * 50.0f; // Adjust this distance based on your needs

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(MarketCart); // Ignore the pawn itself

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			return true; // Right back wheel is grounded
		}
	}

	// Check for the left back wheel
	if (MarketCart->BLWheel)
	{
		Start = MarketCart->BLWheel->GetComponentLocation(); // Get the location of the back left wheel
		End = Start - FVector(0, 0, 1) * 50.0f; // Same distance as above
        
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(MarketCart); // Ignore the pawn itself

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			return true; // Left back wheel is grounded
		}
	}

	return false; // No back wheels 
}
void UCartMovementComponent::StartBoost(bool IsReversed)
{
	if (!bCanBoost || bIsBoosting || CurrentState == ECartState::SlowingDown)
	{
		return; // Prevent boosting if on cooldown or already boosting
	}
	if (!IsGrounded() || !IsCartUpright(0))
	{
		return;
	}
	bIsBoosting = true;
	bCanBoost = false;
	AnimPushDirection = IsReversed;
	AnimbIsPushing = true;
	ApplyImpulse(IsReversed);
	// Set a timer to reset the boost state
	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().SetTimer(BoostCooldownTimerHandle, this, &UCartMovementComponent::ResetBoostCooldown, BoostCooldown, false);
	}

	// Optionally, you can add some logic to stop boosting visually or apply effects.
}
void UCartMovementComponent::ApplyImpulse(bool IsReversed)
{
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
	{
		FVector ForwardVector = UpdatedComponent->GetForwardVector();
		FVector Impulse = ForwardVector * BoostSpeed;
		if(IsReversed)
		{
			Impulse  *= -1;
			Impulse /= 2;
		}// BoostSpeed is now treated as the impulse magnitude
		PrimitiveComponent->AddImpulse(Impulse, NAME_None, true);
	}
}


void UCartMovementComponent::ResetBoostCooldown()
{
	bIsBoosting = false;// Reset the boost state
	bCanBoost = true;
	AnimbIsPushing = false;
}
void UCartMovementComponent::StartSlowDown()
{
	bIsSlowingDown = true;
	AnimbIsSlowingDown = true;
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
	{
		PrimitiveComponent->SetLinearDamping(SlowDownFactor);
	}
}

void UCartMovementComponent::StopSlowDown()
{
	bIsSlowingDown = false;
	AnimbIsSlowingDown = false;
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
	{
		PrimitiveComponent->SetLinearDamping(0);
	}
}

void UCartMovementComponent::TurnCart(float TurnIntensity)
{
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
	{
		FVector CurVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();
		if (CurrentState != ECartState::Toppled && !bIsSlowingDown)
		{
			float SteeringInput = TurnIntensity;
			FVector TorqueToAdd = FVector(0, 0, SteeringInput * TurnRate * 0.01f);
			if (CurVelocity.IsNearlyZero(50.0f))
			{
				TorqueToAdd *= 5;
			}
			PrimitiveComponent->AddTorqueInRadians(TorqueToAdd, NAME_None, true);
			if(TurnIntensity != 0)
			{
				PrimitiveComponent->SetLinearDamping(0.8f);
			}
			else
			{
				PrimitiveComponent->SetLinearDamping(0.0f);
			}
			AnimTurnIntensity = TurnIntensity;
			
		}
	}
}

void UCartMovementComponent::SetPushingAnimationToFalse()
{
	AnimbIsPushing = false;
}


