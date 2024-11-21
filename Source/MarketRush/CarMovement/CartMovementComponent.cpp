// Fill out your copyright notice in the Description page of Project Settings.


#include "CartMovementComponent.h"


// Sets default values for this component's properties
UCartMovementComponent::UCartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	BoostSpeed = 2000.f;
	BoostCooldown = 2.0f;
	bIsBoosting = false;
	bCanBoost = true;
	MaxVelocity = 400.0f;
	Speed = 60000.0f;
	TurnRate = 200.0f;
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


