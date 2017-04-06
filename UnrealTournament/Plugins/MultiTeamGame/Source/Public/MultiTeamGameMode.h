/**
 * Base class for Multi Team game modes
 * Created by Brian 'Snake' Alexander, 2017
 */
#pragma once

#include "UnrealTournament.h"
#include "MultiTeamGameState.h"
#include "MultiTeamGameMode.generated.h"

const int32 MAX_NUM_TEAMS = 4;

UCLASS(Abstract, Config = MultiTeamGame)
class MULTITEAMGAME_API AMultiTeamGameMode : public AUTTeamGameMode
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintReadOnly, Category = MultiTeam)
	AMultiTeamGameState* MTeamGameState;

	/** Used by the UI to set the NumTeams value */
	UPROPERTY(Config, EditDefaultsOnly, BlueprintReadOnly, Category = MultiTeam)
	int32 NumOfTeams;

	/** The main color used in each teams material skins. Value is given to MultiTeamGameState for replication. Array index == TeamNum */
	UPROPERTY(Config, EditDefaultsOnly, Category = MultiTeam)
	FLinearColor TeamBodySkinColor[4];
	
	/** The team overlay color used in each teams material skins. Value is given to MultiTeamGameState for replication. Array index == TeamNum */
	UPROPERTY(Config, EditDefaultsOnly, Category = MultiTeam)
	FLinearColor TeamSkinOverlayColor[4];

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual void AnnounceMatchStart() override;
	virtual bool ChangeTeam(AController* Player, uint8 NewTeam = 255, bool bBroadcast = true) override;
	virtual bool MovePlayerToTeam(AController* Player, AUTPlayerState* PS, uint8 NewTeam) override;
	virtual void PlayEndOfMatchMessage() override;
	virtual void CreateGameURLOptions(TArray<TSharedPtr<TAttributePropertyBase>>& MenuProps) override;
	void BuildServerResponseRules(FString& OutRules) override;

#if !UE_SERVER
	virtual void CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers) override;
	virtual void BuildScoreInfo(AUTPlayerState* PlayerState, TSharedPtr<class SUTTabWidget> TabWidget, TArray<TSharedPtr<struct TAttributeStat> >& StatList) override;
#endif

};
