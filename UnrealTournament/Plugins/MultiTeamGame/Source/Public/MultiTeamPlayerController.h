/**
 * Adds 4 team support
 * Created by Brian 'Snake' Alexander, 2017
 */
#pragma once

#include "UnrealTournament.h"
#include "UTPlayerController.h"
#include "MultiTeamPlayerController.generated.h"

UCLASS()
class MULTITEAMGAME_API AMultiTeamPlayerController : public AUTPlayerController
{
	GENERATED_UCLASS_BODY()

	virtual void ServerSwitchTeam_Implementation() override;
};
