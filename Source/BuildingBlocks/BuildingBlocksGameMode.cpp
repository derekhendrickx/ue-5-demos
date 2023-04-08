// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildingBlocksGameMode.h"
#include "BuildingBlocksCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABuildingBlocksGameMode::ABuildingBlocksGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
