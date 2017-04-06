// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "MultiTeamGameMode.h"
#include "MultiTeamTeamInfo.h"
#include "UTMutator.h"
#include "MultiTeamScoreboard.h"
#include "UTHUD_MultiTeam.h"
#include "StatNames.h"
#include "MultiTeamPlayerState.h"
#include "MultiTeamGameMessage.h"
#include "MultiTDMGameMode.h"

AMultiTDMGameMode::AMultiTDMGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bScoreSuicides = true;
	bScoreTeamKills = true;
	bForceRespawn = true;
	bGameHasImpactHammer = true;
	HUDClass = AUTHUD_MultiTeam::StaticClass();
	DisplayName = NSLOCTEXT("MultiTeamGameMode", "T4DM", "Multi Team Deathmatch");
	XPMultiplier = 3.0f;
}

void AMultiTDMGameMode::ScoreTeamKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType)
{
	AUTPlayerState* KillerState = (Killer != NULL) ? Cast<AUTPlayerState>(Killer->PlayerState) : NULL;
	AUTPlayerState* VictimState = (Other != NULL) ? Cast<AUTPlayerState>(Other->PlayerState) : NULL;
	if (VictimState && VictimState->Team && KillerState && KillerState->Team)
	{
		if (bScoreTeamKills)
		{
			int32 ScoreChange = -1;
			KillerState->AdjustScore(ScoreChange); // @TODO FIXMESTEVE track team kills
			KillerState->Team->Score += ScoreChange;
			KillerState->Team->ForceNetUpdate();
		}
	}

	AddKillEventToReplay(Killer, Other, DamageType);

	if (BaseMutator != NULL)
	{
		BaseMutator->ScoreKill(Killer, Other, DamageType);
	}
}

void AMultiTDMGameMode::ScoreKill_Implementation(AController* Killer, AController* Other, APawn* KilledPawn, TSubclassOf<UDamageType> DamageType)
{
	AUTPlayerState* KillerState = (Killer != NULL) ? Cast<AUTPlayerState>(Killer->PlayerState) : NULL;
	AUTPlayerState* VictimState = (Other != NULL) ? Cast<AUTPlayerState>(Other->PlayerState) : NULL;
	if (VictimState != NULL && VictimState->Team != NULL)
	{
		int32 ScoreChange = 0;
		if (Killer == NULL || Killer == Other)
		{
			if (bScoreSuicides)
			{
				ScoreChange = -1;
			}
		}
		else if (MTeamGameState->OnSameTeam(Killer, Other))
		{
			if (bScoreTeamKills)
			{
				ScoreChange = -1;
			}
		}
		else
		{
			ScoreChange = 1;
		}

		if (KillerState != NULL && KillerState->Team != NULL)
		{
			KillerState->Team->Score += ScoreChange;
			KillerState->Team->ForceNetUpdate();
			if (!bHasBroadcastDominating)
			{
				int32 BestScore = 0;
				for (int32 i = 0; i < Teams.Num(); i++)
				{
					if ((Teams[i] != KillerState->Team) && (Teams[i]->Score >= BestScore))
					{
						BestScore = Teams[i]->Score;
					}
				}
				if (KillerState->Team->Score >= BestScore + 20)
				{
					bHasBroadcastDominating = true;
					BroadcastLocalized(this, UMultiTeamGameMessage::StaticClass(), 10, KillerState, NULL, KillerState->Team);
				}
			}
		}
		else
		{
			VictimState->Team->Score += ScoreChange;
			VictimState->Team->ForceNetUpdate();
		}
	}

	Super::ScoreKill_Implementation(Killer, Other, KilledPawn, DamageType);
}

AUTPlayerState* AMultiTDMGameMode::IsThereAWinner_Implementation(bool& bTied)
{
	AUTTeamInfo* BestTeam = NULL;
	bTied = false;
	for (int32 i=0; i < MTeamGameState->Teams.Num(); i++)
	{
		if (MTeamGameState->Teams[i] != NULL)
		{
			if (BestTeam == NULL || MTeamGameState->Teams[i]->Score > BestTeam->Score)
			{
				BestTeam = MTeamGameState->Teams[i];
				bTied = false;
			}
			else if (MTeamGameState->Teams[i]->Score == BestTeam->Score)
			{
				bTied = true;
			}
		}
	}

	AUTPlayerState* BestPlayer = NULL;
	if (!bTied)
	{
		float BestScore = 0.0;

		for (int32 PlayerIdx=0; PlayerIdx < MTeamGameState->PlayerArray.Num();PlayerIdx++)
		{
			AUTPlayerState* PS = Cast<AUTPlayerState>(MTeamGameState->PlayerArray[PlayerIdx]);
			if (PS != NULL && PS->Team == BestTeam)
			{
				if (BestPlayer == NULL || PS->Score > BestScore)
				{
					BestPlayer = PS;
					BestScore = BestPlayer->Score;
				}
			}
		}
	}
	return BestPlayer;
}

void AMultiTDMGameMode::UpdateSkillRating()
{
	ReportRankedMatchResults(NAME_TDMSkillRating.ToString());
}

uint8 AMultiTDMGameMode::GetNumMatchesFor(AUTPlayerState* PS, bool InbRankedSession) const
{
	return PS ? PS->TDMMatchesPlayed : 0;
}

int32 AMultiTDMGameMode::GetEloFor(AUTPlayerState* PS, bool InbRankedSession) const
{
	return PS ? PS->TDMRank : Super::GetEloFor(PS, InbRankedSession);
}

void AMultiTDMGameMode::SetEloFor(AUTPlayerState* PS, bool InbRankedSession, int32 NewEloValue, bool bIncrementMatchCount)
{
	if (PS)
	{
		PS->TDMRank = NewEloValue;
		if (bIncrementMatchCount && (PS->TDMMatchesPlayed < 255))
		{
			PS->TDMMatchesPlayed++;
		}
	}
}