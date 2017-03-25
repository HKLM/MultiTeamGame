// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTMultiTeamTypes.h"
#include "MultiTeamTeamInfo.h"
#include "MultiTeamGameState.generated.h"

UCLASS()
class MULTITEAMGAME_API AMultiTeamGameState : public AUTGameState
{
	GENERATED_UCLASS_BODY()

	/** 
	 * The main color used in each teams material skins. 
	 * Replicated Value is set by MultiTeamGame::InitGameState() 
	 * Array index == TeamNum
	 */
	UPROPERTY(Replicated, EditAnywhere, Category = TeamSkins)
	FLinearColor TeamBodySkinColor[4];

	/**
	 * The team overlay color used in each teams material skins. 
	 * Replicated Value is set by MultiTeamGame::InitGameState() 
	 * Array index == TeamNum
	 */
	UPROPERTY(Replicated, EditAnywhere, Category = TeamSkins)
	FLinearColor TeamSkinOverlayColor[4];

private:
	/**
	 * Used to limit updating team skins to every other tick
	 */
	UPROPERTY(Transient)
	bool bSkipThisTick;

public:

	UFUNCTION(BlueprintCallable, Category = MultiTeamGameState)
	TEnumAsByte<EControllingMultiTeam::Type> GetLeadingTeam() const;

	/**
	 * Finds the current winning team, or NULL if tied.
	 * @return	AMultiTeamTeamInfo	The Current winning team or NULL if tied at zero
	 */
	UFUNCTION(BlueprintCallable, Category = MultiTeamGameState)
	virtual AUTTeamInfo* FindLeadingTeam();

	/**
	 * Finds the player with the highest score from the team specified in TeamNumToTest
	 * @param	TeamNumToTest	TeamIndex of the team to check.
	 * @return	AUTPlayerState	The player with highest score
	 */
	UFUNCTION(BlueprintCallable, Category = MultiTeamGameState)
	virtual AUTPlayerState* FindBestPlayerOnTeam(uint8 TeamNumToTest);

	/** 
	 * Returns the TeamBodySkinColor for the given TeamIndex 
	 * @param	TeamIndex		TeamIndex of the team to lookup
	 * @return	FLinearColor	The TeamBodySkinColor for the givin TeamIndex
	 */
	UFUNCTION(BlueprintCallable, Category = TeamSkins)
	inline FLinearColor GetTeamSkinColor(uint8 TeamIndex) const { return TeamBodySkinColor[TeamIndex]; };

	/** 
	 * Returns the TeamSkinOverlayColor for the given TeamIndex 
	 * @param	TeamIndex		TeamIndex of the team to lookup
	 * @return	FLinearColor	The TeamSkinOverlayColor for the givin TeamIndex
	 */
	UFUNCTION(BlueprintCallable, Category = TeamSkins)
	inline FLinearColor GetTeamSkinOverlayColor(uint8 TeamIndex) const { return TeamSkinOverlayColor[TeamIndex]; };

	virtual void SetWinner(AUTPlayerState* NewWinner) override;
	virtual FText GetGameStatusText(bool bForScoreboard = false) override;

	virtual void Tick(float DeltaTime) override;
	virtual void UpdateHighlights_Implementation() override;
};
