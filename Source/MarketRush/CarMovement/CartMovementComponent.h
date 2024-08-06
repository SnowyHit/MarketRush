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

	void SetMovementMode(bool bIsFastMode);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float SlowSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float FastSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float SlowTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cart Movement")
	float FastTurnRate;

	bool bIsFastMode;
};
