// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTHUD.h"
#include "UTHUD_MultiTeam.generated.h"

UCLASS()
class MULTITEAMGAME_API AUTHUD_MultiTeam : public AUTHUD
{
	GENERATED_UCLASS_BODY()

	// Icons to display on minimap
	UPROPERTY(EditAnywhere, Category = "HUD")
	FVector2D MultiTeamIconUV[5];

	// Custom HUDAtlas based off original, but adds 4 team logos
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
	UTexture2D* HUDMultiTeamAtlas;

	virtual void BeginPlay() override;
	virtual FLinearColor GetBaseHUDColor() override;
	FLinearColor GetWidgetTeamColor();

	/**
	 * Lookup FColor for the givin Team
	 * @param	TeamIndex	the team to lookup
	 * @return	FColor		color of the team
	 */
	virtual FColor GetColorByTeamIndex(int32 TeamIndex);

	/**
	 * Lookup FLinearColor for the givin Team
	 * @param	TeamIndex		the team to lookup
	 * @return	FLinearColor	color of the team
	 */
	virtual FLinearColor GetLinearColorByTeamIndex(int32 TeamIndex);
};
