/**
 * Multi Team DM Game
 * Created by Brian 'Snake' Alexander, 2017
 */
#pragma once

#include "UnrealTournament.h"
#include "MultiTeamGameMode.h"
#include "MultiTDMGameMode.generated.h"

UCLASS()
class MULTITEAMGAME_API AMultiTDMGameMode : public AMultiTeamGameMode
{
	GENERATED_UCLASS_BODY()

	/** whether to reduce team score for team kills */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MultiTeamDM)
	bool bScoreTeamKills;

	/** whether to reduce team score for suicides */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = MultiTeamDM)
	bool bScoreSuicides;

	virtual void ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType) override;
	virtual void ScoreTeamKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType) override;
	virtual AUTPlayerState* IsThereAWinner_Implementation(bool& bTied) override;
	virtual void UpdateSkillRating() override;
	virtual uint8 GetNumMatchesFor(AUTPlayerState* PS, bool bRankedSession) const override;
	virtual int32 GetEloFor(AUTPlayerState* PS, bool bRankedSession) const override;
	virtual void SetEloFor(AUTPlayerState* PS, bool bRankedSession, int32 NewEloValue, bool bIncrementMatchCount) override;
};
