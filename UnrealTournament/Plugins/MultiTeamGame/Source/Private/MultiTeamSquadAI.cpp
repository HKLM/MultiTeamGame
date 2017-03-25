// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "MultiTeamGameState.h"
#include "MultiTeamGameMode.h"
#include "UTSquadAI.h"
#include "MultiTeamSquadAI.h"
#include "UTDefensePoint.h"

FName NAME_Attack(TEXT("Attack"));
FName NAME_Defend(TEXT("Defend"));
FName NAME_Roam(TEXT("Roam"));
FName NAME_Backup(TEXT("Backup"));

AMultiTeamSquadAI::AMultiTeamSquadAI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bGotObjectives = false;
}

void AMultiTeamSquadAI::Initialize(AUTTeamInfo* InTeam, FName InOrders)
{
	Super::Initialize(InTeam, InOrders);

	for (TActorIterator<AUTGameObjective> It(GetWorld()); It; ++It)
	{
		if (!It->bHidden)
		{
			TheObjectives.AddUnique(*It);
		}
	}
	if (TheObjectives.Num() == 0 || !TheObjectives.IsValidIndex(0))
	{
		bNoObjectivesInMap = true;
		return;
	}
	int8 i = 0;

	if (Orders == NAME_Attack)
	{
		i = 0;
	}
	else if (Orders == NAME_Defend)
	{
		i = TheObjectives.IsValidIndex(1) ? 1 : 0;
	}
	else if (Orders == NAME_Roam)
	{
		i = TheObjectives.IsValidIndex(2) ? 2 : 0;
	}
	else
	{
		if (TheObjectives.IsValidIndex(TheObjectives.Num() - 1))
		{
			i = TheObjectives.Num() - 1;
		}
		else if (TheObjectives.IsValidIndex(0))
		{
			i = 0;
		}
	}
	// Extra validation to ensure game does not crash
	if (TheObjectives.IsValidIndex(i))
	{
		SetObjective(TheObjectives[i]);
		for (AController* C : Members)
		{
			AUTBot* B = Cast<AUTBot>(C);
			if (B != nullptr)
			{
				B->WhatToDoNext();
			}
		}
	}
}

void AMultiTeamSquadAI::BeginPlay()
{
	Super::BeginPlay();
	FindAllObjectives();
}

bool AMultiTeamSquadAI::CheckSquadObjectives(AUTBot* B)
{
	if (bNoObjectivesInMap)
	{
		return Super::CheckSquadObjectives(B);
	}
	FString BotOrderString = "";
	FName CurrentOrders = GetCurrentOrders(B);
	if (CurrentOrders == NAME_Defend)
	{
		SetDefensePointFor(B);
	}
	else
	{
		B->SetDefensePoint(NULL);
	}

	if (!bGotObjectives || GameObjective == NULL)
	{
		FindAllObjectives();
	}

	AUTGameObjective* tmpObjective = nullptr;

	int8 Lottery = FMath::RandRange(0, 10);
	bool bLottery = Lottery > 5 ? true : false;
	if (B->NeedsWeapon() && B->FindInventoryGoal(0.0f))
	{
		B->GoalString = FString::Printf(TEXT("Get inventory %s"), *GetNameSafe(B->RouteCache.Last().Actor.Get()));
		B->SetMoveTarget(B->RouteCache[0]);
		B->StartWaitForMove();
		return true;
	}
	else if (GameObjective != nullptr && GameObjective->GetTeamNum() != B->GetTeamNum()
			 && B->LineOfSightTo(GameObjective)
			 && ((GameObjective->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 700.0f))
	{
		BotOrderString = FString::Printf(TEXT("%s-Goto Objective LineOfSight: %s"), *CurrentOrders.ToString(), *GetNameSafe(GameObjective));
		return B->TryPathToward(GameObjective, bLottery, true, BotOrderString);
	}
	else if (CurrentOrders == NAME_Attack || CurrentOrders == NAME_Roam)
	{
		if (GameObjective != nullptr && GameObjective->GetTeamNum() != B->GetTeamNum())
		{
			BotOrderString = FString::Printf(TEXT("%s-Goto objective : %s"), *CurrentOrders.ToString(), *GetNameSafe(GameObjective));
			return B->TryPathToward(GameObjective, false, true, BotOrderString);
		}
		else if (CurrentOrders == NAME_Roam && (!B->LostContact(2.0f) || MustKeepEnemy(B, B->GetEnemy())))
		{
			B->GoalString = "Roam-Fight";
			return false;
		}
		else if (CheckSuperPickups(B, 5000))
		{
			return true;
		}
		else if (GameObjective != nullptr && GameObjective->GetTeamNum() == B->GetTeamNum() && GetWorld()->GetTimeSeconds() - 10.0f > LastObjectiveChange)
		{
			tmpObjective = GetNewObjective(GameObjective, B);
			if (tmpObjective != nullptr)
			{
				SetObjective(tmpObjective);
				BotOrderString = FString::Printf(TEXT("%s-Got New objective : %s"), *CurrentOrders.ToString(), *GetNameSafe(tmpObjective));
				return B->TryPathToward(tmpObjective, bLottery, true, BotOrderString);
			}
		}
		else if (!B->LostContact(2.0f) || MustKeepEnemy(B, B->GetEnemy()))
		{
			B->GoalString = "ATTACK-Fight";
			return false;
		}
	}
	else if (CurrentOrders == NAME_Defend || CurrentOrders == NAME_Backup)
	{
		if (CurrentOrders == NAME_Defend  && GameObjective->GetTeamNum() == B->GetTeamNum() && GetWorld()->GetTimeSeconds() - 120.0f > LastObjectiveChange)
		{
			tmpObjective = GetNewObjective(GameObjective, B);
			if (tmpObjective != nullptr)
			{
				SetObjective(tmpObjective);
				BotOrderString = FString::Printf(TEXT("%s-Got New Objective : %s"), *CurrentOrders.ToString(), *GetNameSafe(tmpObjective));
				return B->TryPathToward(tmpObjective, false, true, BotOrderString);
			}
		}
		else if (GameObjective != nullptr && GameObjective->GetTeamNum() != B->GetTeamNum())
		{
			BotOrderString = FString::Printf(TEXT("%s-Goto objective : %s"), *CurrentOrders.ToString(), *GetNameSafe(GameObjective));
			return B->TryPathToward(GameObjective, bLottery, true, BotOrderString);
		}
		else if (!B->LostContact(2.0f) || MustKeepEnemy(B, B->GetEnemy()))
		{
			B->GoalString = "DEFEND-Fight";
			return false;
		}
		else if (CurrentOrders == NAME_Backup && GameObjective != nullptr && GameObjective->GetTeamNum() == B->GetTeamNum() && GetWorld()->GetTimeSeconds() - 30.0f > LastObjectiveChange)
		{
			tmpObjective = GetNewObjective(GameObjective, B);
			if (tmpObjective != nullptr)
			{
				SetObjective(tmpObjective);
				BotOrderString = FString::Printf(TEXT("%s-Got New objective : %s"), *CurrentOrders.ToString(), *GetNameSafe(tmpObjective));
				return B->TryPathToward(tmpObjective, false, true, BotOrderString);
			}
		}
		else if (CheckSuperPickups(B, 5000))
		{
			return true;
		}
		else if (B->GetDefensePoint() != nullptr)
		{
			BotOrderString = FString::Printf(TEXT("%s-Goto defense point : %s"), *CurrentOrders.ToString(), *B->GetDefensePoint()->GetName());
			return B->TryPathToward(B->GetDefensePoint(), true, true, BotOrderString);
		}
	}

	return Super::CheckSquadObjectives(B);
}

bool AMultiTeamSquadAI::IsNearEnemyBase(const FVector& TestLoc)
{
	for (uint8 i = 0; i < TheObjectives.Num(); i++)
	{
		if ((TestLoc - TheObjectives[i]->GetActorLocation()).Size() < 2500.0f)
		{
			return true;
		}
	}
	return false;
}

AUTGameObjective* AMultiTeamSquadAI::GetNewObjective(AActor* OldObjective, AUTBot* B)
{
	LastObjectiveChange = GetWorld()->GetTimeSeconds();
	AUTGameObjective* BestObjective = nullptr;
	AUTGameObjective* TheOldObjective = Cast<AUTGameObjective>(OldObjective);
	if (TheObjectives.Num() < 2)
	{
		FindAllObjectives();
	}
	// randomly pick random objective
	int8 randomObjective = FMath::RandRange(0, TheObjectives.Num() - 1);
	int8 LotteryNumber = FMath::RandRange(0, TheObjectives.Num() - 1);
	if (randomObjective == LotteryNumber && TheOldObjective != TheObjectives[LotteryNumber])
	{
		return TheObjectives[LotteryNumber];
	}

	if (B != nullptr)
	{
		if (TheOldObjective != nullptr)
		{
			for (uint8 i = 0; i < TheObjectives.Num(); i++)
			{
				// Find a near by control point that our team does not control
				if ((TheOldObjective != TheObjectives[i]
					&& TheObjectives[i]->GetTeamNum() != B->GetTeamNum()
					&& B->LineOfSightTo(TheObjectives[i]))
					|| ((TheObjectives[i]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 3000.0f))
				{
					BestObjective = TheObjectives[i];
					break;
				}
			}
			if (BestObjective == nullptr)
			{
				// Find just a control point that our team does not control
				for (uint8 j = 0; j < TheObjectives.Num(); j++)
				{
					if (TheOldObjective != TheObjectives[j]
						&& TheObjectives[j]->GetTeamNum() != B->GetTeamNum())
					{
						BestObjective = TheObjectives[j];
						break;
					}
				}
				// Find a control point that is a differant then the original one
				if (BestObjective == nullptr)
				{
					for (uint8 l = 0; l < TheObjectives.Num(); l++)
					{
						if (TheOldObjective != TheObjectives[l])
						{
							BestObjective = TheObjectives[l];
							break;
						}
					}
				}
			}
		}
		if (BestObjective == nullptr)
		{
			for (uint8 p = 0; p < TheObjectives.Num(); p++)
			{
				// Find a near by control point that our team does not control
				if (TheObjectives[p]->GetTeamNum() != B->GetTeamNum()
					&& (B->LineOfSightTo(TheObjectives[p])
					|| (TheObjectives[p]->GetActorLocation() - B->GetPawn()->GetActorLocation()).Size() < 4000.0f))
				{
					BestObjective = TheObjectives[p];
					break;
				}
			}
		}
		// Last resort, pick anything
		if (BestObjective == nullptr)
		{
			for (uint8 k = 0; k < TheObjectives.Num(); k++)
			{
				BestObjective = TheObjectives[k];
				break;
			}
		}
	}
	return BestObjective;
}

void AMultiTeamSquadAI::FindAllObjectives()
{
	TheObjectives.Empty();
	for (TActorIterator<AUTGameObjective> It(GetWorld()); It; ++It)
	{
		//if (!It->bEnabled)
		//{
			TheObjectives.AddUnique(*It);
		//}
	}
	if (TheObjectives.Num() == 0)
	{
		bNoObjectivesInMap = true;
	}
	else
	{
		bGotObjectives = true;
	}
}

void AMultiTeamSquadAI::NotifyObjectiveEvent(AActor* InObjective, AController* InstigatedBy, FName EventName)
{
	if (!bNoObjectivesInMap)
	{
		AUTGameObjective* InGameObjective = Cast<AUTGameObjective>(InObjective);
		if (InstigatedBy != NULL && InGameObjective != NULL
			&& InGameObjective->GetCarriedObjectHolder() == InstigatedBy->PlayerState
			&& Members.Contains(InstigatedBy))
		{
			// re-enable alternate paths for flag carrier so it can consider them for planning its escape
			AUTBot* B = Cast<AUTBot>(InstigatedBy);
			if (B != nullptr)
			{
				B->bDisableSquadRoutes = false;
				B->SquadRouteGoal.Clear();
				B->UsingSquadRouteIndex = INDEX_NONE;
			}
			SetLeader(InstigatedBy);
		}
	}

	Super::NotifyObjectiveEvent(InObjective, InstigatedBy, EventName);
}

AUTGameObjective* AMultiTeamSquadAI::GetNearestObjective(AUTBot* InstigatedBy, bool bOnlyNotControlled)
{
	AUTGameObjective* BestObjective = nullptr;
	float bestDistance = 100000.0f;

	FMultiAITargets f;
	TArray<FMultiAITargets> CP;

	for (uint8 i = 0; i < TheObjectives.Num(); i++)
	{
		if ((bOnlyNotControlled && (TheObjectives[i]->GetTeamNum() != InstigatedBy->GetTeamNum()))
			|| !bOnlyNotControlled)
		{
			f.Target = TheObjectives[i];
			f.Distance = (TheObjectives[i]->GetActorLocation() - InstigatedBy->GetPawn()->GetActorLocation()).Size();
			CP.Add(f);
		}
	}
	if (!CP.IsValidIndex(0))
	{
		// Last time nothing was found, so lets add then all without restriction
		for (uint8 i = 0; i < TheObjectives.Num(); i++)
		{
			if (TheObjectives.IsValidIndex(i))
			{
				f.Target = TheObjectives[i];
				f.Distance = (TheObjectives[i]->GetActorLocation() - InstigatedBy->GetPawn()->GetActorLocation()).Size();
				CP.Add(f);
			}
		}
	}
	if (CP.IsValidIndex(0))
	{
		for (uint8 i = 0; i < CP.Num(); i++)
		{
			if (i == 0)
			{
				bestDistance = CP[i].Distance;
				BestObjective = CP[i].Target;
			}
			else if (CP[i].Distance < bestDistance)
			{
				bestDistance = CP[i].Distance;
				BestObjective = CP[i].Target;
			}
		}
	}
	return BestObjective;
}
