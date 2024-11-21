// Fill out your copyright notice in the Description page of Project Settings.


#include "CartMovementComponent.h"


// Sets default values for this component's properties
UCartMovementComponent::UCartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bIsBoosting = false;
	bCanBoost = true;
	MaxVelocity = 400.0f;
	Speed = 60000.0f;
	TurnRate = 1000.0f;
	SlowDownFactor = 20.0f;
	BoostSpeed = 500.0f;
	BoostCooldown = 0.5f;// Reduce velocity to 50%
	bIsSlowingDown = false;
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
	
	if (bIsSlowingDown)
	{
		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
		{
			FVector CurrentVelocity = PrimitiveComponent->GetPhysicsLinearVelocity();
	    
			// Apply a friction-like force only if the cart is moving
			if (!CurrentVelocity.IsNearlyZero())
			{
				FVector CurrentVel = PrimitiveComponent->GetPhysicsLinearVelocity();
				SlowDownFactor = FMath::Clamp(SlowDownFactor, 0.0f, 1.0f);
				FVector Deceleration = CurrentVel * SlowDownFactor * DeltaTime; // Gradual slowdown
    
				// Apply deceleration, ensuring the cart doesn't reverse direction suddenly
				FVector NewVelocity = CurrentVel - Deceleration;
    
				// Apply the new velocity back to the cart
				PrimitiveComponent->SetPhysicsLinearVelocity(NewVelocity, false);
			}
			else
			{
				FVector ForwardVector = UpdatedComponent->GetForwardVector();
            
				// Apply a small reverse force along the cart's forward direction (opposite of forward)
				FVector ReverseForce = -ForwardVector * ReverseSpeed;  // Apply reverse in the direction opposite to forward
            
				// Apply the reverse force to the cart
				PrimitiveComponent->AddForce(ReverseForce, NAME_None, false);
			}
		}
		
	}
	auto CurrentInputVector = GetPendingInputVector().GetClampedToMaxSize(1.f);
	if (!CurrentInputVector.IsNearlyZero())
    {
        if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
        {
        	/*// Calculate the cart's forward direction
        	FVector ForwardVector = UpdatedComponent->GetForwardVector();
        	FVector SurfaceNormal = FVector::UpVector; // Default to world up

        	// Check for the surface normal beneath the cart
        	FHitResult Hit;
        	FVector Start = UpdatedComponent->GetComponentLocation();
        	FVector End = Start - FVector(0, 0, 100); // Check 100 units below the cart

        	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
        	{
        		SurfaceNormal = Hit.ImpactNormal;
        	}

        	// Adjust the right vector to account for the surface normal
        	FVector AdjustedRightVector = FVector::CrossProduct(SurfaceNormal, ForwardVector).GetSafeNormal();

        	// Calculate torque using the adjusted right vector
        	float SteeringInput = CurrentInputVector.Y;
        	FVector TorqueToAdd = AdjustedRightVector * SteeringInput * TurnRate * DeltaTime;

        	// Apply torque
        	PrimitiveComponent->AddTorqueInRadians(TorqueToAdd, NAME_None, true);*/
        	
        	// Adjust steering input
        	float SteeringInput = CurrentInputVector.Y;
        	
        	FVector TorqueToAdd = FVector(0, 0, SteeringInput * TurnRate * DeltaTime);
		    
        	PrimitiveComponent->AddTorqueInRadians(TorqueToAdd, NAME_None, true);
        }
    }
	
	ConsumeInputVector();
}

void UCartMovementComponent::StartBoost()
{
	if (!bCanBoost || bIsBoosting)
	{
		return; // Prevent boosting if on cooldown or already boosting
	}

	bIsBoosting = true;
	bCanBoost = false;

	// Apply forward impulse to the cart
	if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
	{
		FVector ForwardVector = UpdatedComponent->GetForwardVector();
		FVector Impulse = ForwardVector * BoostSpeed; // BoostSpeed is now treated as the impulse magnitude
		PrimitiveComponent->AddImpulse(Impulse, NAME_None, true);
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
	bIsBoosting = false; // Reset the boost state
	bCanBoost = true; 
}
void UCartMovementComponent::StartSlowDown()
{
	bIsSlowingDown = true;
}

void UCartMovementComponent::StopSlowDown()
{
	bIsSlowingDown = false;
}


