// Fill out your copyright notice in the Description page of Project Settings.


#include "CartMovementComponent.h"


// Sets default values for this component's properties
UCartMovementComponent::UCartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MaxVelocityFast = 400.0f;
	MaxVelocitySlow = 250.0f;
	MaxAngularVelocityFast = 200.0f;
	MaxAngularVelocitySlow = 100.0f;
	SlowSpeed = 60000.0f;
	FastSpeed = 120000.0f;
	SlowTurnRate = 500.0f;
	FastTurnRate = 250.0f;
	bIsFastMode = false;
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
	bool bIsMovingForward = true;
	if (!CurrentInputVector.IsNearlyZero())
    {
        float CurrentSpeed = bIsFastMode ? FastSpeed : SlowSpeed;
        FVector ForwardVector = UpdatedComponent->GetForwardVector();

        // Calculate the force to apply
        FVector ForceToAdd = ForwardVector * CurrentInputVector.X * CurrentSpeed * DeltaTime;
		

        if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
        {
        	float CurrentVelocityMagnitude = FVector::DotProduct(PrimitiveComponent->GetComponentVelocity(), ForwardVector);
        	
        	// Limit the velocity if necessary
        	if (CurrentVelocityMagnitude > (bIsFastMode ? MaxVelocityFast : MaxVelocitySlow))
        	{
        		// Calculate the scaling factor to ensure max velocity
        		float ScalingFactor = (bIsFastMode ? MaxVelocityFast : MaxVelocitySlow) / CurrentVelocityMagnitude;
        		ForceToAdd *= ScalingFactor;
        	}
            // Apply force to the root component
            PrimitiveComponent->AddForce(ForceToAdd, NAME_None, true);

            // Determine movement direction
            bIsMovingForward = CurrentInputVector.X >= 0;

            // Adjust steering input
            float SteeringInput = CurrentInputVector.Y;
            if (!bIsMovingForward)
            {
                SteeringInput = -CurrentInputVector.Y; // Reverse steering if moving backwards
            }
        	// Apply torque for turning
        	float UsingFastTurnRate = FastTurnRate ;
        	float UsingSlowTurnRate = SlowTurnRate ;
        	if(CurrentVelocityMagnitude < 0.3f)
        	{
        		UsingFastTurnRate = FastTurnRate * 2;
        		UsingSlowTurnRate = SlowTurnRate * 2;
        	}
        	FVector TorqueToAdd = FVector(0, 0, SteeringInput * (bIsFastMode ? UsingFastTurnRate : UsingSlowTurnRate) * DeltaTime);
		    
        	PrimitiveComponent->AddTorqueInRadians(TorqueToAdd, NAME_None, true);
        	PrimitiveComponent->SetPhysicsMaxAngularVelocityInRadians((bIsFastMode ? MaxAngularVelocityFast : MaxAngularVelocitySlow));

            // Debugging the component's physics state
            UE_LOG(LogTemp, Warning, TEXT("Component Velocity: %s"), *PrimitiveComponent->GetComponentVelocity().ToString());
        	UE_LOG(LogTemp, Warning, TEXT("Current Velocity Magnitude: %f"), CurrentVelocityMagnitude);

        }
    }
	
	ConsumeInputVector();
}

void UCartMovementComponent::SetMovementMode(bool isFastMode)
{
	bIsFastMode = isFastMode;
}


