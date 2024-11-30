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
	TurnRate = 1000.0f;
	SlowDownFactor = 20.0f;
	BoostSpeed = 700.0f;
	BoostCooldown = 0.5f;
	MaxRoll = 50.0f;
	MaxPitch = 50.0f;
	// Reduce velocity to 50%
	bIsSlowingDown = false;
	
	CurrentState = ECartState::Idle;
	// ...
}


// Called every frame
void UCartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}
	// Check if the cart is grounded and upright
	FRotator CurrentRotation = PawnOwner->GetActorRotation();
	// Check if at least one back wheel is touching the ground
	bool bIsGrounded = IsGrounded();
	// Check if the cart is upright
	bool bIsUpright = IsCartUpright(0);
	UpdateCartState(bIsGrounded, bIsUpright);
	// Debug information
	// FString DebugMessage = FString::Printf(
	// 	TEXT("Current Rotation: Pitch: %.2f, Roll: %.2f | Grounded: %s | Upright: %s"),
	// 	CurrentRotation.Pitch, CurrentRotation.Roll,
	// 	bIsGrounded ? TEXT("True") : TEXT("False"),
	// 	bIsUpright ? TEXT("True") : TEXT("False")
	// );
	// // Log to the output log
	// UE_LOG(LogTemp, Log, TEXT("%s"), *DebugMessage);
	
	FString StateName = StaticEnum<ECartState>()->GetValueAsString(CurrentState);
	UE_LOG(LogTemp, Log, TEXT("Cart State: %s"), *StateName);
	
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
	{
		FVector CurrentVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();
		// Clamp the current velocity if it exceeds the maximum allowed
		if (CurrentVelocity.Size() > MaxVelocity)
		{
			CurrentVelocity = CurrentVelocity.GetSafeNormal() * MaxVelocity;
			PrimitiveComponent->SetPhysicsLinearVelocity(CurrentVelocity, false);
		}
		if (bIsSlowingDown)
		{
			// Apply a friction-like force only if the cart is moving
			if (!CurrentVelocity.IsNearlyZero(0.1f))
			{
				FVector CurrentVel = PrimitiveComponent->GetPhysicsLinearVelocity();
				SlowDownFactor = FMath::Clamp(SlowDownFactor, 0.0f, 1.0f);
				FVector Deceleration = CurrentVel * SlowDownFactor * DeltaTime; // Gradual slowdown

				// Apply deceleration, ensuring the cart doesn't reverse direction suddenly
				FVector NewVelocity = CurrentVel - Deceleration;

				// Apply the new velocity back to the cart
				PrimitiveComponent->SetPhysicsLinearVelocity(NewVelocity, false);
			}
		}
		auto CurrentInputVector = GetPendingInputVector().GetClampedToMaxSize(1.f);
		if (!CurrentInputVector.IsNearlyZero() && CurrentState != ECartState::Toppled)
		{
			if(!bIsSlowingDown)
			{
				float SteeringInput = CurrentInputVector.Y;
			
				FVector TorqueToAdd = FVector(0, 0, SteeringInput * TurnRate * DeltaTime);
				if (CurrentVelocity.IsNearlyZero(100.0f))
				{
					TorqueToAdd *= 100;
				}
			
				PrimitiveComponent->AddTorqueInRadians(TorqueToAdd, NAME_None, true);
			}
			
		}
		// Update animation variables
		AnimData.Speed = CurrentVelocity.Size();
		// Calculate turn intensity based on input
		FVector InputVector = GetPendingInputVector();
		AnimData.TurnIntensity = InputVector.Y;
    }
	
	ConsumeInputVector();
}

bool UCartMovementComponent::IsFrontWheelLifted() const
{
	FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();
	return CurrentRotation.Pitch > MaxPitch / 2; // Adjust the threshold as needed
}

void UCartMovementComponent::UpdateCartState(bool bIsGrounded, bool bIsUpright)
{
	FVector InputVector = GetPendingInputVector();

	// Reset to Idle if no conditions are met
	CurrentState = ECartState::Upright;

	// Check conditions and update state accordingly
	if (!bIsGrounded)
	{
		CurrentState = ECartState::NotGrounded;
	}
	else if (!bIsUpright)
	{
		CurrentState = ECartState::Toppled;
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

	// Debug output for testing
	UE_LOG(LogTemp, Log, TEXT("Current Cart State: %s"), *UEnum::GetValueAsString(CurrentState));
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

	// Apply forward impulse to the cart
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
		
		AnimData.bIsPushing = true;
	}

	// Set a timer to reset the boost state
	if (GetOwner())
	{
		GetOwner()->GetWorldTimerManager().SetTimer(BoostCooldownTimerHandle, this, &UCartMovementComponent::ResetBoostCooldown, BoostCooldown, false);
	}

	// Optionally, you can add some logic to stop boosting visually or apply effects.
}

void UCartMovementComponent::ResetBoostCooldown()
{
	bIsBoosting = false;// Reset the boost state
	bCanBoost = true;
	AnimData.bIsPushing = false;
}
void UCartMovementComponent::StartSlowDown()
{
	bIsSlowingDown = true;
	AnimData.bIsSlowingDown = true;
}

void UCartMovementComponent::StopSlowDown()
{
	bIsSlowingDown = false;
	AnimData.bIsSlowingDown = false;
}

void UCartMovementComponent::SetPushingAnimationToFalse()
{
	AnimData.bIsPushing = false;
}


