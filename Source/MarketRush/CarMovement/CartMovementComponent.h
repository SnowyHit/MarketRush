// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "CartMovementComponent.generated.h"
USTRUCT(BlueprintType)
struct FCartAnimData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsPushing;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	float TurnIntensity;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	float Speed;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool bIsSlowingDown;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	bool PushDirection;

	FCartAnimData()
		: bIsPushing(false), TurnIntensity(0.0f), Speed(0.0f), bIsSlowingDown(false)
	{}
};
	
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
	Crashed UMETA(DisplayName = "Crashed"),
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
	void TransitionToCrashed();
	void ResetCart();
	void RaiseFrontWheels();
	bool IsCartUpright(float Tolerance) const;
	bool IsGrounded() const;

	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float TurnRate;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	FCartAnimData AnimData;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float MaxVelocity;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float BoostSpeed;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float BoostCooldown;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float SlowDownFactor;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	bool bIsSlowingDown;
	// Boost state
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	bool bIsBoosting;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	bool bCanBoost;
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	double MaxPitch;
	
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	double MaxRoll;
	UPROPERTY(Replicated ,VisibleAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	ECartState CurrentState;
	bool bIsReversedPush;
	FTimerHandle ToppledTimerHandle;
	
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float ToppledDuration;
	// Boost functions
	void StartBoost(bool IsReversed);
	void ApplyImpulse(bool IsReversed);
	void StartSlowDown();
	void StopSlowDown();
	UFUNCTION(BlueprintCallable)
	void SetPushingAnimationToFalse();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartBoost(bool IsReversed);
	void ServerStartBoost_Implementation(bool IsReversed);
	bool ServerStartBoost_Validate(bool IsReversed);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerResetCart();
	void ServerResetCart_Implementation();
	bool ServerResetCart_Validate();

private:
	// Timer handles for managing boost state
	FTimerHandle BoostDurationTimerHandle;
	FTimerHandle BoostCooldownTimerHandle;

	void ResetBoostCooldown();
};
