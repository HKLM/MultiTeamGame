// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "MultiTeamGameState.h"
#include "MultiTeamTeamInfo.h"
#include "MultiTeamSquadAI.h"
#include "UTHUD_MultiTeam.h"
#include "MultiTeamGameMessage.h"
#include "StatNames.h"
#include "UTMultiTeamTypes.h"
#include "UTFirstBloodMessage.h"
#include "UTMutator.h"
#include "Private/Slate/Widgets/SUTTabWidget.h"
#include "Private/Slate/Dialogs/SUTPlayerInfoDialog.h"
#include "SNumericEntryBox.h"
#include "UTPickup.h"
#include "UTPickupHealth.h"
#include "UTRemoteRedeemer.h"
#include "UTRecastNavMesh.h"
#include "UTPathBuilderInterface.h"
#include "MultiTeamPlayerController.h"
#include "MultiTeamPlayerState.h"
#include "MultiTeamVictoryMessage.h"
#include "MultiTeamGameMode.h"

AMultiTeamGameMode::AMultiTeamGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AMultiTeamGameState::StaticClass();
	TeamClass = AMultiTeamTeamInfo::StaticClass();
	SquadType = AMultiTeamSquadAI::StaticClass();
	HUDClass = AUTHUD_MultiTeam::StaticClass();
	GameMessageClass = UMultiTeamGameMessage::StaticClass();

	PlayerPawnObject.Reset();
	PlayerPawnObject = FStringAssetReference(TEXT("/MultiTeamGame/MultiTeamGameContent/DefaultCharacter_MultiTeam.DefaultCharacter_MultiTeam_C"));
	PlayerStateClass = AMultiTeamPlayerState::StaticClass();
	PlayerControllerClass = AMultiTeamPlayerController::StaticClass();
	VictoryMessageClass = UMultiTeamVictoryMessage::StaticClass();
	MapPrefix = TEXT("DM");

	bDelayedStart = false;

	bGameHasImpactHammer = true;
	bAllowURLTeamCountOverride = true;
	NumOfTeams = 4;
	NumTeams = 4;
	MaxSquadSize = 2;
	bHideInUI = false;

	TeamBodySkinColor[0] = FLinearColor(4.6f, 0.1f, 0.1f, 1.0f);
	TeamBodySkinColor[1] = FLinearColor(0.1f, 0.1f, 4.6f, 1.0f);
	TeamBodySkinColor[2] = FLinearColor(0.01f, 1.0f, 0.01f, 1.0f);
	TeamBodySkinColor[3] = FLinearColor(2.6f, 2.6f, 0.01f, 1.0f);

	TeamSkinOverlayColor[0] = FLinearColor(7.0f, 0.02f, 0.02f, 1.0f);
	TeamSkinOverlayColor[1] = FLinearColor(0.04f, 0.04f, 7.4f, 1.0f);
	TeamSkinOverlayColor[2] = FLinearColor(0.02f, 6.0f, 0.02f, 1.0f);
	TeamSkinOverlayColor[3] = FLinearColor(5.5f, 5.5f, 0.02f, 1.0f);
}

void AMultiTeamGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::AUTGameMode::InitGame(MapName, Options, ErrorMessage);

	bBalanceTeams = !bDevServer && !bOfflineChallenge && EvalBoolOptions(UGameplayStatics::ParseOption(Options, TEXT("BalanceTeams")), bBalanceTeams);

	NumTeams = NumOfTeams;
	NumTeams = UGameplayStatics::GetIntOption(Options, TEXT("NumTeams"), NumTeams);
	NumTeams = FMath::Clamp<uint8>(NumTeams, 2, MAX_NUM_TEAMS);
	NumOfTeams = NumTeams;
	if (TeamClass == NULL)
	{
		TeamClass = AMultiTeamTeamInfo::StaticClass();
	}
	for (uint8 i = 0; i < NumTeams; i++)
	{
		AMultiTeamTeamInfo* NewTeam = GetWorld()->SpawnActor<AMultiTeamTeamInfo>(TeamClass);
		NewTeam->TeamIndex = i;
		if (TeamColors.IsValidIndex(i))
		{
			NewTeam->TeamColor = TeamColors[i];
		}
		if (TeamNames.IsValidIndex(i))
		{
			NewTeam->TeamName = TeamNames[i];
		}

		Teams.Add(NewTeam);
		checkSlow(Teams[i] == NewTeam);
	}
	MercyScore = FMath::Max(0, UGameplayStatics::GetIntOption(Options, TEXT("MercyScore"), MercyScore));
}

void AMultiTeamGameMode::InitGameState()
{
	Super::InitGameState();
	MTeamGameState = Cast<AMultiTeamGameState>(GameState);
	if (MTeamGameState)
	{
		MTeamGameState->NumTeams = NumTeams;
		//Notify gamestate of TeamsSkin colors to use
		for (uint8 i = 0; i < 4; i++)
		{
			MTeamGameState->TeamBodySkinColor[i] = TeamBodySkinColor[i];
			MTeamGameState->TeamSkinOverlayColor[i] = TeamSkinOverlayColor[i];
		}
	}
}

void AMultiTeamGameMode::AnnounceMatchStart()
{
	if (bAnnounceTeam)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AUTPlayerController* NextPlayer = Cast<AUTPlayerController>(*Iterator);
			AUTTeamInfo* Team = (NextPlayer && Cast<AUTPlayerState>(NextPlayer->PlayerState)) ? Cast<AUTPlayerState>(NextPlayer->PlayerState)->Team : NULL;
			if (Team)
			{
				NextPlayer->ClientReceiveLocalizedMessage(GameMessageClass, Team->TeamIndex + 9, NextPlayer->PlayerState, NULL, NULL);
			}
		}
	}
	else
	{
		BroadcastLocalized(this, UUTGameMessage::StaticClass(), 0, NULL, NULL, NULL);
	}
}

bool AMultiTeamGameMode::ChangeTeam(AController* Player, uint8 NewTeam, bool bBroadcast)
{
	if (Player == NULL)
	{
		return false;
	}
	else
	{
		AMultiTeamPlayerState* PS = Cast<AMultiTeamPlayerState>(Player->PlayerState);
		if (PS == nullptr || PS->bOnlySpectator)
		{
			return false;
		}
		else
		{
			if ((bOfflineChallenge || bBasicTrainingGame) && PS->Team)
			{
				return false;
			}

			bool bForceTeam = false;
			if (!Teams.IsValidIndex(NewTeam))
			{
				bForceTeam = true;
			}
			else
			{
				if (GetNetMode() != NM_Standalone)
				{
					// see if someone is willing to switch
					for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
					{
						AMultiTeamPlayerController* NextPlayer = Cast<AMultiTeamPlayerController>(*Iterator);
						AMultiTeamPlayerState* SwitchingPS = NextPlayer ? Cast<AMultiTeamPlayerState>(NextPlayer->PlayerState) : NULL;
						if (SwitchingPS && SwitchingPS->bPendingTeamSwitch && (SwitchingPS->Team == Teams[NewTeam]) && Teams.IsValidIndex(1 - NewTeam))
						{
							// Found someone who wants to leave team, so just replace them
							MovePlayerToTeam(NextPlayer, SwitchingPS, 1 - NewTeam);
							SwitchingPS->HandleTeamChanged(NextPlayer);
							MovePlayerToTeam(Player, PS, NewTeam);
							return true;
						}
					}
				}

				if (ShouldBalanceTeams(PS->Team == NULL) || (bBalanceTeams && Cast<AUTBot>(Player)))
				{
					for (int32 i = 0; i < Teams.Num(); i++)
					{
						// if this isn't smallest team, use PickBalancedTeam() to get right team
						if (i != NewTeam && Teams[i]->GetSize() <= Teams[NewTeam]->GetSize())
						{
							bForceTeam = true;
							break;
						}
					}
				}
			}
			if (bForceTeam)
			{
				NewTeam = PickBalancedTeam(PS, NewTeam);
			}
		
			if (MovePlayerToTeam(Player, PS, NewTeam))
			{
				AMultiTeamPlayerController* PC = Cast<AMultiTeamPlayerController>(Player);
				if (PC && !HasMatchStarted() && bUseTeamStarts)
				{
					AActor* const StartSpot = FindPlayerStart(PC);
					if (StartSpot != NULL)
					{
						PC->StartSpot = StartSpot;
						PC->ViewStartSpot();
					}
				}
				return true;
			}

			PS->bPendingTeamSwitch = true;
			PS->ForceNetUpdate();
			return false;
		}
	}
}

bool AMultiTeamGameMode::MovePlayerToTeam(AController* Player, AUTPlayerState* PS, uint8 NewTeam)
{
	AMultiTeamPlayerState* PSD = Cast<AMultiTeamPlayerState>(PS);
	if (Teams.IsValidIndex(NewTeam) && PSD && (PSD->Team == NULL || PSD->Team->TeamIndex != NewTeam))
	{
		//Make sure we kill the player before they switch sides so the correct team loses the point
		AUTCharacter* UTC = Cast<AUTCharacter>(Player->GetPawn());
		if (UTC != nullptr)
		{
			UTC->PlayerSuicide();
		}
		if (PSD->Team != nullptr)
		{
			PSD->Team->RemoveFromTeam(Player);
		}
		Teams[NewTeam]->AddToTeam(Player);
		PSD->bPendingTeamSwitch = false;
		PSD->ForceNetUpdate();

		PSD->MakeTeamSkin(NewTeam);
		// Clear the player's gameplay mute list.

		AMultiTeamPlayerController* PlayerController = Cast<AMultiTeamPlayerController>(Player);

		if (PlayerController && MTeamGameState)
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				AMultiTeamPlayerController* NextPlayer = Cast<AMultiTeamPlayerController>(*Iterator);
				if (NextPlayer)
				{
					TSharedPtr<const FUniqueNetId> Id = NextPlayer->PlayerState->UniqueId.GetUniqueNetId();
					bool bIsMuted = Id.IsValid() && PlayerController->IsPlayerMuted(Id.ToSharedRef().Get());

					bool bOnSameTeam = MTeamGameState->OnSameTeam(PlayerController, NextPlayer);
					if (bIsMuted && bOnSameTeam) 
					{
						PlayerController->GameplayUnmutePlayer(NextPlayer->PlayerState->UniqueId);
						NextPlayer->GameplayUnmutePlayer(PlayerController->PlayerState->UniqueId);
					}
					if (!bIsMuted && !bOnSameTeam) 
					{
						PlayerController->GameplayMutePlayer(NextPlayer->PlayerState->UniqueId);
						NextPlayer->GameplayMutePlayer(PlayerController->PlayerState->UniqueId);
					}
					
				}
			}
		}

		return true;
	}
	return false;
}

void AMultiTeamGameMode::PlayEndOfMatchMessage()
{
	if (UTGameState && UTGameState->WinningTeam)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AUTPlayerController* PC = Cast<AUTPlayerController>(*Iterator);
			if (PC && (PC->PlayerState != NULL) && !PC->PlayerState->bOnlySpectator)
			{
				PC->ClientReceiveLocalizedMessage(VictoryMessageClass,
													UTGameState->WinningTeam->GetTeamNum(),
													UTGameState->WinnerPlayerState,
													PC->PlayerState,
													UTGameState->WinningTeam);
			}
		}
	}
}


void AMultiTeamGameMode::BuildServerResponseRules(FString& OutRules)
{
	OutRules += FString::Printf(TEXT("Goal Score\t%i\t"), GoalScore);
	OutRules += FString::Printf(TEXT("Time Limit\t%i\t"), int32(TimeLimit / 60.0));
	OutRules += FString::Printf(TEXT("Teams\t%i\t"), NumTeams);

	AUTMutator* Mut = BaseMutator;
	while (Mut)
	{
		OutRules += FString::Printf(TEXT("Mutator\t%s\t"), *Mut->DisplayName.ToString());
		Mut = Mut->NextMutator;
	}
}

void AMultiTeamGameMode::CreateGameURLOptions(TArray<TSharedPtr<TAttributePropertyBase>>& MenuProps)
{
	Super::CreateGameURLOptions(MenuProps);
	MenuProps.Add(MakeShareable(new TAttributeProperty<int32>(this, &NumOfTeams, TEXT("NumTeams"))));
}

#if !UE_SERVER
void AMultiTeamGameMode::CreateConfigWidgets(TSharedPtr<class SVerticalBox> MenuSpace, bool bCreateReadOnly, TArray< TSharedPtr<TAttributePropertyBase> >& ConfigProps, int32 MinimumPlayers)
{
	Super::CreateConfigWidgets(MenuSpace, bCreateReadOnly, ConfigProps, MinimumPlayers);

	TSharedPtr< TAttributeProperty<int32> > NumOfTeamsAttr = StaticCastSharedPtr<TAttributeProperty<int32>>(FindGameURLOption(ConfigProps, TEXT("NumTeams")));

	if (NumOfTeamsAttr.IsValid())
	{
		MenuSpace->AddSlot()
		.AutoHeight()
		.VAlign(VAlign_Top)
		.Padding(0.0f,0.0f,0.0f,5.0f)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.WidthOverride(350)
				[
					SNew(STextBlock)
					.TextStyle(SUWindowsStyle::Get(),"UT.Common.NormalText")
					.Text(NSLOCTEXT("MultiTeamGame", "NumTeams", "Teams"))
				]
			]
			+ SHorizontalBox::Slot()
			.Padding(20.0f,0.0f,0.0f,0.0f)
			.AutoWidth()
			[
				SNew(SBox)
				.WidthOverride(300)
				[
					bCreateReadOnly ?
					StaticCastSharedRef<SWidget>(
						SNew(STextBlock)
						.TextStyle(SUWindowsStyle::Get(),"UT.Common.ButtonText.White")
						.Text(NumOfTeamsAttr.ToSharedRef(), &TAttributeProperty<int32>::GetAsText)
					) :
					StaticCastSharedRef<SWidget>(
						SNew(SNumericEntryBox<int32>)
						.Value(NumOfTeamsAttr.ToSharedRef(), &TAttributeProperty<int32>::GetOptional)
						.OnValueChanged(NumOfTeamsAttr.ToSharedRef(), &TAttributeProperty<int32>::Set)
						.AllowSpin(true)
						.Delta(1)
						.MinValue(2)
						.MaxValue(MAX_NUM_TEAMS)
						.MinSliderValue(2)
						.MaxSliderValue(MAX_NUM_TEAMS)
						.EditableTextBoxStyle(SUWindowsStyle::Get(), "UT.Common.NumEditbox.White")
					)
				]
			]
		];
	}
}

void AMultiTeamGameMode::BuildScoreInfo(AUTPlayerState* PlayerState, TSharedPtr<class SUTTabWidget> TabWidget, TArray<TSharedPtr<TAttributeStat> >& StatList)
{
	TAttributeStat::StatValueTextFunc TwoDecimal = [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> FText
	{
		return FText::FromString(FString::Printf(TEXT("%8.2f"), Stat->GetValue()));
	};

	TSharedPtr<SVerticalBox> LeftPane;
	TSharedPtr<SVerticalBox> RightPane;
	TSharedPtr<SHorizontalBox> HBox;
	BuildPaneHelper(HBox, LeftPane, RightPane);

	TabWidget->AddTab(NSLOCTEXT("AUTGameMode", "Score", "Score"), HBox);

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AMultiTeamGameMode", "Score", "Player Score"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->Score;	})), StatList);

	LeftPane->AddSlot().AutoHeight()[SNew(SBox).HeightOverride(40.0f)];
	LeftPane->AddSlot().AutoHeight()[SNew(SBox)
		.HeightOverride(50.0f)
		[
			SNew(STextBlock)
			.TextStyle(SUWindowsStyle::Get(), "UT.Common.BoldText")
		.Text(NSLOCTEXT("AMultiTeamGameMode", "Scoring", " SCORING "))
		]
	];

	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AMultiTeamGameMode", "RegularKillPoints", "Score from Frags"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->GetStatsValue(NAME_RegularKillPoints);	})), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "Kills", "Kills"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float { return PS->Kills;	})), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "Deaths", "Deaths"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float {	return PS->Deaths; })), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "Suicides", "Suicides"), MakeShareable(new TAttributeStat(PlayerState, NAME_Suicides)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "ScorePM", "Score Per Minute"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float
	{
		return (PS->StartTime < PS->GetWorld()->GetGameState<AMultiTeamGameState>()->ElapsedTime) ? PS->Score * 60.f / (PS->GetWorld()->GetGameState<AMultiTeamGameState>()->ElapsedTime - PS->StartTime) : 0.f;
	}, TwoDecimal)), StatList);
	NewPlayerInfoLine(LeftPane, NSLOCTEXT("AUTGameMode", "KDRatio", "K/D Ratio"), MakeShareable(new TAttributeStat(PlayerState, NAME_None, [](const AUTPlayerState* PS, const TAttributeStat* Stat) -> float
	{
		return (PS->Deaths > 0) ? float(PS->Kills) / PS->Deaths : 0.f;
	}, TwoDecimal)), StatList);
}

#endif
