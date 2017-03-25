// Created by Brian 'Snake' Alexander, 2017
#pragma  once

#include "UnrealTournament.h"
#include "UTLineUpZoneVisualizationCharacter.h"
#include "MultiTeamLineUpZoneVisualizationCharacter.generated.h"

UCLASS(transient)
class MULTITEAMGAME_API AMultiTeamLineUpZoneVisualizationCharacter : public AUTLineUpZoneVisualizationCharacter
{
	GENERATED_UCLASS_BODY()

	void OnChangeTeamNum();
};