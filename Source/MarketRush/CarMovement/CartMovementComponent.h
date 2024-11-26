// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CartMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECartState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Turning     UMETA(DisplayName = "Turning"),
	Wheelie     UMETA(DisplayName = "Wheelie"),
	Upright     UMETA(DisplayName = "Upright"),
	Toppled     UMETA(DisplayName = "Toppled"),
	NotGrounded UMETA(DisplayName = "Not Grounded"),
	SlowingDown UMETA(DisplayName = "Slowing Down"),
	Boosting    UMETA(DisplayName = "Boosting"),
	TurningRight UMETA(DisplayName = "Turning Right"),
	TurningLeft UMETA(DisplayName = "Turning Left"),
};
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARKETRUSH_API UCartMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UCartMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool IsFrontWheelLifted() const;
	void UpdateCartState(bool bIsGrounded, bool bIsUpright);
	void RaiseFrontWheels();
	bool IsCartUpright(float Tolerance) const;
	bool IsGrounded() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float TurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float MaxVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float BoostSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float BoostCooldown;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float SlowDownFactor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	bool bIsSlowingDown;

	// Boost state
	bool bIsBoosting;
	bool bCanBoost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	double MaxPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	double MaxRoll;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	ECartState CurrentState;

	// Boost functions
	void StartBoost(bool IsReversed);
	void StartSlowDown();
	void StopSlowDown();

private:
	// Timer handles for managing boost state
	FTimerHandle BoostDurationTimerHandle;
	FTimerHandle BoostCooldownTimerHandle;

	void ResetBoostCooldown();
};
