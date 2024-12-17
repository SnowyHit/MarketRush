// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
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

	UPROPERTY(Replicated ,BlueprintReadWrite, Category = "Animation")
	bool AnimbIsPushing;

	UPROPERTY(Replicated ,BlueprintReadWrite, Category = "Animation")
	float AnimTurnIntensity;

	UPROPERTY(Replicated ,BlueprintReadWrite, Category = "Animation")
	float AnimSpeed;

	UPROPERTY(Replicated ,BlueprintReadWrite, Category = "Animation")
	bool AnimbIsSlowingDown;

	UPROPERTY(Replicated ,BlueprintReadWrite, Category = "Animation")
	bool AnimPushDirection;
	
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float ToppledDuration;
	
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	FVector ReplicatedLocation;
	
	UPROPERTY(Replicated ,EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	FRotator ReplicatedRotation;
	// Boost functions
	void StartBoost(bool IsReversed);
	void ApplyImpulse(bool IsReversed);
	void StartSlowDown();
	void StopSlowDown();
	void TurnCart(float TurnIntensity);
	
	UFUNCTION(BlueprintCallable)
	void SetPushingAnimationToFalse();
	
	UFUNCTION(Client, Reliable)
	void ClientStartBoost(bool IsReversed);
	UFUNCTION(Server, Reliable)
	void ServerStartBoost(bool IsReversed);
	
	UFUNCTION(Client, Reliable)
	void ClientSlowDown(bool Start);
	UFUNCTION(Server, Reliable)
	void ServerSlowDown(bool Start);

	UFUNCTION(Client, Reliable)
	void ClientTurnCart(float TurnIntensity);
	UFUNCTION(Server, Reliable)
	void ServerTurnCart(float TurnIntensity);
	
	UFUNCTION(Client, Reliable)
	void ClientResetCart();
	UFUNCTION(Server, Reliable)
	void ServerResetCart();

	UFUNCTION(Client, Reliable)
	void ClientRaiseFrontWheels();
	UFUNCTION(Server, Reliable)
	void ServerRaiseFrontWheels();

	UFUNCTION(Server, Reliable)
	void ServerUpdateCart(const FVector& NewLocation , const FRotator& NewRotation);

	

private:
	// Timer handles for managing boost state
	FTimerHandle BoostDurationTimerHandle;
	FTimerHandle BoostCooldownTimerHandle;

	void ResetBoostCooldown();
};
