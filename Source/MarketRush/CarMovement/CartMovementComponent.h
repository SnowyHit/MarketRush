// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CartMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARKETRUSH_API UCartMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UCartMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float TurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float MaxVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float BoostSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float ReverseSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float BoostCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float SlowDownFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	bool bIsSlowingDown;

	// Boost state
	bool bIsBoosting;
	bool bCanBoost;

	// Boost functions
	void StartBoost();
	void StartSlowDown();
	void StopSlowDown();

private:
	// Timer handles for managing boost state
	FTimerHandle BoostDurationTimerHandle;
	FTimerHandle BoostCooldownTimerHandle;

	void ResetBoostCooldown();
};
