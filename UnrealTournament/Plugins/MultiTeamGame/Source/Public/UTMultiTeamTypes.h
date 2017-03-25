// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTMultiTeamTypes.generated.h"

/**
 * Values that represent a objective's current state
 */
UENUM(BlueprintType)
namespace EControllingMultiTeam
{
	enum Type
	{
		TE_Red		UMETA(DisplayName = "Red"),
		TE_Blue		UMETA(DisplayName = "Blue"),
		TE_Green	UMETA(DisplayName = "Green"),
		TE_Gold		UMETA(DisplayName = "Gold"),

		TE_Neutral	UMETA(DisplayName = "Neutral"),

		TE_Disabled	UMETA(DisplayName = "Disabled")

	};
}
