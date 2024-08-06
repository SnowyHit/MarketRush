// Fill out your copyright notice in the Description page of Project Settings.


#include "CartMovementComponent.h"


// Sets default values for this component's properties
UCartMovementComponent::UCartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SlowSpeed = 60000.0f;
	FastSpeed = 120000.0f;
	SlowTurnRate = 100.0f;
	FastTurnRate = 50.0f;
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
	if (!CurrentInputVector.IsNearlyZero())
	{
		float CurrentSpeed = bIsFastMode ? FastSpeed : SlowSpeed;

		FVector ForwardVector = UpdatedComponent->GetForwardVector();
		FVector ForceToAdd = ForwardVector * CurrentInputVector.X * CurrentSpeed * DeltaTime;

		if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(UpdatedComponent))
		{
			// Apply force to the root component
			PrimitiveComponent->AddForce(ForceToAdd, NAME_None, true);

			// Determine movement direction
			bool bIsMovingForward = CurrentInputVector.X > 0;

			// Adjust steering input
			float SteeringInput = CurrentInputVector.Y;
			if (!bIsMovingForward)
			{
				SteeringInput = -CurrentInputVector.Y; // Reverse steering if moving backwards
			}

			// Apply torque for turning
			FVector TorqueToAdd = FVector(0, 0, SteeringInput * (bIsFastMode ? FastTurnRate : SlowTurnRate) * DeltaTime);
			PrimitiveComponent->AddTorqueInRadians(TorqueToAdd, NAME_None, true);
		}
	}
	
	ConsumeInputVector();
}

void UCartMovementComponent::SetMovementMode(bool isFastMode)
{
	bIsFastMode = isFastMode;
}


