// Copyright Epic Games, Inc. All Rights Reserved.

#include "MarketRushGameMode.h"
#include "MarketRushCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMarketRushGameMode::AMarketRushGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
