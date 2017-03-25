// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "MultiTeamGameMode.h"
#include "MultiTeamGameState.h"
#include "MultiTeamGameMessage.h"
#include "MultiTeamPlayerController.h"
#include "MultiTeamCharacter.h"
#include "UTPlayerState.h"
#include "MultiTeamPlayerState.h"

AMultiTeamPlayerState::AMultiTeamPlayerState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AMultiTeamPlayerState::HandleTeamChanged(AController* Controller)
{
	AUTCharacter* Pawn = GetUTCharacter();
	if (Pawn != nullptr)
	{
		Pawn->PlayerChangedTeam();
	}
	if (Team)
	{
		int32 Switch = Team->TeamIndex + 9;
		AMultiTeamPlayerController* PC = Cast<AMultiTeamPlayerController>(Controller);
		if (PC)
		{
			PC->ClientReceiveLocalizedMessage(UMultiTeamGameMessage::StaticClass(), Switch, this, NULL, NULL);
		}
	}
}

void AMultiTeamPlayerState::ServerRequestChangeTeam_Implementation(uint8 NewTeamIndex)
{
	AMultiTeamGameMode* Game = GetWorld()->GetAuthGameMode<AMultiTeamGameMode>();
	if (Game != nullptr && Game->bTeamGame)
	{
		AController* Controller = Cast<AController>(GetOwner());
		if (Controller != nullptr)
		{
			AMultiTeamPlayerController* MTPController = Cast<AMultiTeamPlayerController>(Controller);
			if (MTPController)
			{
				if (!GetWorld()->GetGameState<AMultiTeamGameState>()->Teams.IsValidIndex(NewTeamIndex) || NewTeamIndex == 255)
				{
					NewTeamIndex = 0;
				}
				if (Game->ChangeTeam(MTPController, NewTeamIndex, true))
				{
					HandleTeamChanged(MTPController);
					MakeTeamSkin(NewTeamIndex);
				}
			}
		}
	}
}

void AMultiTeamPlayerState::MakeTeamSkin(uint8 NewTeamIndex)
{
	AMultiTeamCharacter* UTC = Cast<AMultiTeamCharacter>(GetUTCharacter());
	AMultiTeamGameState* MTGS = Cast<AMultiTeamGameState>(GetWorld()->GetGameState<AUTGameState>());
	if (MTGS && UTC && MTGS->Teams.IsValidIndex(NewTeamIndex))
	{
		/* value what base color to use (red or blue) */
		UTC->TeamBodySkinColor = MTGS->TeamBodySkinColor[NewTeamIndex];
		UTC->TeamSkinOverlayColor = MTGS->TeamSkinOverlayColor[NewTeamIndex];
		UTC->SetTeamSkin(NewTeamIndex);
	}
}
