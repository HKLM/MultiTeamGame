// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "MultiTeamTeamInfo.generated.h"

UCLASS(NotPlaceable)
class MULTITEAMGAME_API AMultiTeamTeamInfo : public AUTTeamInfo
{
	GENERATED_UCLASS_BODY()

protected:
	/** team score as a float value */
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Replicated, Category = MultiTeam)
	float FloatScore;

public:
	/** Adds ScorePoints value to the FloatScore and Score properties */
	UFUNCTION(BlueprintCallable, Category = MultiTeam)
	virtual void SetFloatScore(float ScorePoints);

	/** 
	 * Returns the teams FloatScore value 
	 * @return	The current value of FloatScore
	 */
	UFUNCTION(BlueprintCallable, Category = MultiTeam)
	virtual float GetFloatScore() { return FloatScore; }

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;
	virtual bool AssignToSquad(AController* C, FName Orders, AController* Leader = NULL) override;
	virtual void AssignDefaultSquadFor(AController* C) override;

	virtual int32 GetNextOrderIndex();
};
