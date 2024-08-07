// Fill out your copyright notice in the Description page of Project Settings.


#include "CartMovementComponent.h"


// Sets default values for this component's properties
UCartMovementComponent::UCartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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
	bool bIsMovingForward = true;
	if (!CurrentInputVector.IsNearlyZero())
    {
        float CurrentSpeed = Speed;
        FVector ForwardVector = UpdatedComponent->GetForwardVector();

        // Calculate the force to apply
        FVector ForceToAdd = ForwardVector * CurrentInputVector.X * CurrentSpeed * DeltaTime * 100;
		

        if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
        {
        	float CurrentVelocityMagnitude = FMath::Abs(FVector::DotProduct(PrimitiveComponent->GetComponentVelocity(), ForwardVector));

        	// Limit the velocity if necessary
        	if (CurrentVelocityMagnitude > MaxVelocity)
        	{
        		// Calculate the scaling factor to ensure max velocity
        		float ScalingFactor = MaxVelocity / CurrentVelocityMagnitude;
        		ForceToAdd *= ScalingFactor;
        	}
        	
        	bIsMovingForward = CurrentInputVector.X >= 0;
			if(!bIsMovingForward)
			{
				ForceToAdd*=0.3;
			}
            // Apply force to the root component
            PrimitiveComponent->AddForce(ForceToAdd, NAME_None, true);

            // Determine movement direction
            

            // Adjust steering input
            float SteeringInput = CurrentInputVector.Y;
        	
        	FVector TorqueToAdd = FVector(0, 0, SteeringInput * TurnRate * DeltaTime);
		    
        	PrimitiveComponent->AddTorqueInRadians(TorqueToAdd, NAME_None, true);


        }
    }
	
	ConsumeInputVector();
}


