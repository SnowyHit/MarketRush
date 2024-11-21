// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "MarketCart.generated.h"

class UCartMovementComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class MARKETRUSH_API AMarketCart : public APawn
{
	GENERATED_BODY()

public:
	AMarketCart();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionComp;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* FRWheel;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* FLWheel;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* BRWheel;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* BLWheel;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BoostAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlowDownAction;

protected:
	virtual void BeginPlay() override;


public:
	virtual void Tick(float DeltaTime) override;
	void Move(const FInputActionValue& Value);
	void Boost(const FInputActionValue& Value);
	void StartSlowDown(const FInputActionValue& Value);
	void EndSlowDown(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
