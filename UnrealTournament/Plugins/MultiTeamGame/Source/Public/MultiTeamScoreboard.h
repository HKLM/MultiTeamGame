/**
 * Created by Brian 'Snake' Alexander, 2017
 * 4 Team Scoreboard
 */
#pragma once 

#include "UnrealTournament.h"
#include "UTHUDWidget.h"
#include "UTTeamScoreboard.h"
#include "MultiTeamScoreboard.generated.h"

UCLASS()
class MULTITEAMGAME_API UMultiTeamScoreboard : public UUTTeamScoreboard
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
	FText GreenTeamText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoreboard")
	FText GoldTeamText;
	
	UPROPERTY()
		float GreenScoreScaling;

	UPROPERTY()
		float GoldScoreScaling;

	virtual void Draw_Implementation(float DeltaTime) override;
	virtual void SelectionRight() override;

protected:
	/** Cache NumTeams value from GameState. Used to determin if green and/or gold teams are drawn */
	UPROPERTY()
	int8 GameNumTeams;

	virtual void Draw4ScorePanel(float RenderDelta, float& YOffset);

	virtual void DrawTeamPanel(float RenderDelta, float& YOffset) /*override*/;
	virtual void Draw4TeamPanel(float RenderDelta, float& YOffset);

	virtual void Draw4ScoreHeaders(float RenderDelta, float& DrawY);

	virtual void DrawPlayerScores(float RenderDelta, float& DrawY) /*override*/;
	virtual void Draw4PlayerScores(float RenderDelta, float& DrawY);

	/** 5coring breakdown for Teams. */
	virtual void DrawTeamScoreBreakdown(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom);
	virtual void Draw4TeamScoreBreakdown(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom);

	virtual void DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo) override;
	virtual void Draw4TeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo);

	/** Draw one line of scoring breakdown where values are clock stats. */
	virtual void DrawClockTeamStatsLine(FText StatsName, FName StatsID, float DeltaTime, float XOffset, float& YPos, const FStatsFontInfo& StatsFontInfo, float ScoreWidth, bool bSkipEmpty) override;
};
