// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTSquadAI.h"
#include "MultiTeamSquadAI.generated.h"

USTRUCT()
struct FMultiAITargets
{
	GENERATED_USTRUCT_BODY()

	AUTGameObjective* Target;
	float Distance;
};

UCLASS(NotPlaceable)
class MULTITEAMGAME_API AMultiTeamSquadAI : public AUTSquadAI
{
	GENERATED_UCLASS_BODY()

	/** Array of AUTGameObjective */
	TArray<AUTGameObjective*> TheObjectives;

	virtual void Initialize(AUTTeamInfo* InTeam, FName InOrders) override;
	virtual void BeginPlay() override;
	virtual bool CheckSquadObjectives(AUTBot* B) override;
	virtual bool IsNearEnemyBase(const FVector& TestLoc);

	virtual void SetObjective(AActor* InObjective) override
	{
		if (InObjective != Objective)
		{
			Objective = InObjective;
			GameObjective = Cast<AUTGameObjective>(InObjective);
			SquadRoutes.Empty();
			CurrentSquadRouteIndex = INDEX_NONE;
			for (AController* C : Members)
			{
				AUTBot* B = Cast<AUTBot>(C);
				if (B != NULL)
				{
					B->UsingSquadRouteIndex = INDEX_NONE;
					B->bDisableSquadRoutes = false;
					B->SquadRouteGoal.Clear();
					//B->WhatToDoNext();
				}
			}
		}
	}

	virtual AUTGameObjective* GetNewObjective(AActor* OldObjective, AUTBot* B);

	virtual AUTGameObjective* GetNearestObjective(AUTBot* InstigatedBy, bool bOnlyNotControlled);

	/** Search for UTGameObjective in the map  */
	virtual void FindAllObjectives();
	virtual void NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName) override;

protected:

	/** Internal flag */
	UPROPERTY(Transient)
		bool bGotObjectives;

	UPROPERTY(Transient)
		bool bNoObjectivesInMap;

	/** Internal counter to prevent excessive Objectie changes */
	float LastObjectiveChange;

};
