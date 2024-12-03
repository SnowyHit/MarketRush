// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float TurnRate;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	FCartAnimData AnimData;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	bool bIsBoosting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	bool bCanBoost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	double MaxPitch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	double MaxRoll;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	ECartState CurrentState;
	bool bIsReversedPush;
	FTimerHandle ToppledTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float ToppledDuration;
	// Boost functions
	void StartBoost(bool IsReversed);
	void ApplyImpulse(bool IsReversed);
	void StartSlowDown();
	void StopSlowDown();
	void UpdateCartTick(float DeltaTime);
	UPROPERTY(Replicated)
	FVector ReplicatedInputVector;

	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;
	
	UFUNCTION(BlueprintCallable)
	void SetPushingAnimationToFalse();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartBoost(bool IsReversed);
	void ServerStartBoost_Implementation(bool IsReversed);
	bool ServerStartBoost_Validate(bool IsReversed);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSlowDown(bool Start);
	void ServerSlowDown_Implementation(bool Start);
	bool ServerSlowDown_Validate(bool Start);

	UFUNCTION(Server, Unreliable)
	void ServerTickCart(float DeltaTime);
	void ServerTickCart_Implementation(float DeltaTime);
	bool ServerTickCart_Validate(float DeltaTime);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpdateCartState(const FVector& InputVector, const FRotator& Rotation , const float& Deltatime);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerResetCart();
	void ServerResetCart_Implementation();
	bool ServerResetCart_Validate();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRaiseFrontWheels();
	void ServerRaiseFrontWheels_Implementation();
	bool ServerRaiseFrontWheels_Validate();

private:
	// Timer handles for managing boost state
	FTimerHandle BoostDurationTimerHandle;
	FTimerHandle BoostCooldownTimerHandle;

	void ResetBoostCooldown();
};
