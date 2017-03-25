/**
 * Adds dynamic 4 team skin support
 * Created by Brian 'Snake' Alexander, 2017 using Intellectual Property from RattleSN4K3's UTBetrayal UT4 plugin
 * 
 * MultiTeamCharacter is the main character class for MultiTeams which is handling all the 4 team skin
 * logic. The 2 Blueprint character classes BaseUTCharacter_MultiTeam and DefaultCharacter_MultiTeam
 * are plain copies of the non-MultiTeam classes where BaseUTCharacter_MultiTeam has this class as its parent
 * class.
 * 
 * In order to create clean copies, first duplicate BaseUTCharacter, rename to BaseUTCharacter_MultiTeam
 * and then reparent to AMultiTeamCharacter. 
 * Then duplicate DefaultCharacter, rename to DefaultCharacter_MultiTeam and reparent to BaseUTCharacter_MultiTeam.
 */
#pragma once

#include "UnrealTournament.h"
#include "UTCharacter.h"
#include "MultiTeamCharacter.generated.h"

UCLASS()
class MULTITEAMGAME_API AMultiTeamCharacter : public AUTCharacter
{
	GENERATED_UCLASS_BODY()

	/**
	 * The team body color used by SetTeamSkin() to make the characters team material skin
	 * Replicated Value is set from MultiTeamGameState
	 */
	UPROPERTY(Replicated, VisibleAnywhere, Category = TeamSkins)
	FLinearColor TeamBodySkinColor;

	/**
	 * The team overlay color used by SetTeamSkin() to make the characters team material skin.
	 * Replicated Value is set from MultiTeamGameState
	 */
	UPROPERTY(Replicated, VisibleAnywhere, Category = TeamSkins)
	FLinearColor TeamSkinOverlayColor;

	/**
	 * The team overlay color used to make the characters team material skin.
	 * @param	NewTeamIndex	The TeamIndex of team to make skin color for
	 */
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Set Team Skin", Tooltip="Set the characters team skin color to match the given TeamIndex.", Keywords="teamskin skin"), Category = TeamSkins)
	virtual void SetTeamSkin(uint8 NewTeamIndex);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
