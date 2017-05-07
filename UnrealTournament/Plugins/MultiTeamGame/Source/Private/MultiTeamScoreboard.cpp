// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTHUDWidget.h"
#include "UTTeamScoreboard.h"
#include "MultiTeamGameState.h"
#include "UTDemoRecSpectator.h"
#include "StatNames.h"
#include "MultiTeamScoreboard.h"

UMultiTeamScoreboard::UMultiTeamScoreboard(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GreenTeamText = NSLOCTEXT("MultiTeamScoreboard", "GreenTeam", "GREEN");
	GoldTeamText = NSLOCTEXT("MultiTeamScoreboard", "GoldTeam", "GOLD");

	Size = FVector2D(1920.0f, 1000.0f);
}

void UMultiTeamScoreboard::Draw_Implementation(float RenderDelta)
{
	Super::UUTHUDWidget::Draw_Implementation(RenderDelta);

	AMultiTeamGameState* GameState = Cast<AMultiTeamGameState>(UTGameState);
	if (GameState == nullptr) return;
	GameNumTeams = GameState->NumTeams;

	float YOffset = 48.f*RenderScale;
	DrawGamePanel(RenderDelta, YOffset);
	DrawTeamPanel(RenderDelta, YOffset);
	if (UTGameState != nullptr && UTGameState->GetMatchState() != MatchState::CountdownToBegin && UTGameState->GetMatchState() != MatchState::PlayerIntro)
	{
		DrawScorePanel(RenderDelta, YOffset);
	}

	if (GameNumTeams > 2)
	{
		Draw4TeamPanel(RenderDelta, YOffset);
		if (UTGameState != nullptr && UTGameState->GetMatchState() != MatchState::CountdownToBegin && UTGameState->GetMatchState() != MatchState::PlayerIntro)
		{
			Draw4ScorePanel(RenderDelta, YOffset);
		}
	}

	if (ShouldDrawScoringStats())
	{
		DrawScoringStats(RenderDelta, YOffset);
	}
	else
	{
		DrawCurrentLifeStats(RenderDelta, YOffset);
	}

	if (UTHUDOwner && UTHUDOwner->bDisplayMatchSummary && !bIsInteractive)
	{
		DrawMatchSummary(RenderDelta);
	}
	else
	{
		DrawMinimap(RenderDelta);
	}
}

void UMultiTeamScoreboard::Draw4ScorePanel(float RenderDelta, float& YOffset)
{
	if (bIsInteractive)
	{
		SelectionStack.Empty();
	}
	LastScorePanelYOffset = YOffset;
	if (UTGameState)
	{
		Draw4ScoreHeaders(RenderDelta, YOffset);
		Draw4PlayerScores(RenderDelta, YOffset);
	}
}

void UMultiTeamScoreboard::DrawTeamPanel(float RenderDelta, float& YOffset)
{
	if (UTGameState == nullptr || UTGameState->Teams.Num() < 2 || UTGameState->Teams[0] == nullptr || UTGameState->Teams[1] == nullptr) return;

	float Width = 0.5f * (Size.X - 400.f) * RenderScale;

	float FrontSize = 35.f * RenderScale;
	float EndSize = 16.f * RenderScale;
	float MiddleSize = Width - FrontSize - EndSize;
	float BackgroundY = YOffset + 22.f * RenderScale;
	float TeamTextY = YOffset + 40.f * RenderScale;
	float TeamScoreY = YOffset + 36.f * RenderScale;
	float BackgroundHeight = 65.f * RenderScale;
	float TeamEdgeSize = 40.f * RenderScale;
	float NamePosition = TeamEdgeSize + FrontSize + 0.25f*MiddleSize;

	// Draw the Background
	DrawTexture(UTHUDOwner->ScoreboardAtlas, TeamEdgeSize, BackgroundY, FrontSize, BackgroundHeight, 0, 188, 36, 65, 1.0, UTGameState->Teams[0]->TeamColor);
	DrawTexture(UTHUDOwner->ScoreboardAtlas, TeamEdgeSize + FrontSize, BackgroundY, MiddleSize, BackgroundHeight, 39, 188, 64, 65, 1.0, UTGameState->Teams[0]->TeamColor);
	DrawTexture(UTHUDOwner->ScoreboardAtlas, TeamEdgeSize + FrontSize + MiddleSize, BackgroundY, EndSize, BackgroundHeight, 39, 188, 64, 65, 1.0, UTGameState->Teams[0]->TeamColor);

	DrawText(FText::AsNumber(UTGameState->Teams[0]->Score), NamePosition, TeamScoreY, UTHUDOwner->HugeFont, false, FVector2D(0, 0), FLinearColor::Black, true, FLinearColor::Black, 1.5f*RenderScale, 1.f, FLinearColor::White, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), ETextHorzPos::Right, ETextVertPos::Center);

	float LeftEdge = Canvas->ClipX - TeamEdgeSize - FrontSize - MiddleSize - EndSize;

	DrawTexture(UTHUDOwner->ScoreboardAtlas, LeftEdge + EndSize + MiddleSize, BackgroundY, FrontSize, BackgroundHeight, 196, 188, 36, 65, 1.f, UTGameState->Teams[1]->TeamColor);
	DrawTexture(UTHUDOwner->ScoreboardAtlas, LeftEdge + EndSize, BackgroundY, MiddleSize, BackgroundHeight, 130, 188, 64, 65, 1.f, UTGameState->Teams[1]->TeamColor);
	DrawTexture(UTHUDOwner->ScoreboardAtlas, LeftEdge, BackgroundY, EndSize, BackgroundHeight, 117, 188, 16, 65, 1.f, UTGameState->Teams[1]->TeamColor);

	DrawText(FText::AsNumber(UTGameState->Teams[1]->Score), Canvas->ClipX - NamePosition, TeamScoreY, UTHUDOwner->HugeFont, false, FVector2D(0.f, 0.f), FLinearColor::Black, true, FLinearColor::Black, 1.5f*RenderScale, 1.f, FLinearColor::White, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), ETextHorzPos::Left, ETextVertPos::Center);

	YOffset += 119.f * RenderScale;
}

void UMultiTeamScoreboard::Draw4TeamPanel(float RenderDelta, float& YOffset)
{
	if (UTGameState == NULL || UTGameState->Teams.Num() < 2 || (UTGameState->NumTeams > 2 && UTGameState->Teams[2] == nullptr) || (UTGameState->NumTeams == 4 && UTGameState->Teams[3] == nullptr)) return;

	float Width = 0.5f * (Size.X - 400.f) * RenderScale;

	float FrontSize = 35.f * RenderScale;
	float EndSize = 16.f * RenderScale;
	float MiddleSize = Width - FrontSize - EndSize;
	float BackgroundY = YOffset + 22.f * RenderScale;
	float TeamTextY = YOffset + 40.f * RenderScale;
	float TeamScoreY = YOffset + 36.f * RenderScale;
	float BackgroundHeight = 65.f * RenderScale;
	float TeamEdgeSize = 40.f * RenderScale;
	float NamePosition = TeamEdgeSize + FrontSize + 0.25f*MiddleSize;

	// Draw the Background
	DrawTexture(UTHUDOwner->ScoreboardAtlas, TeamEdgeSize, BackgroundY, FrontSize, BackgroundHeight, 0, 188, 36, 65, 1.0, UTGameState->Teams[2]->TeamColor);
	DrawTexture(UTHUDOwner->ScoreboardAtlas, TeamEdgeSize + FrontSize, BackgroundY, MiddleSize, BackgroundHeight, 39, 188, 64, 65, 1.0, UTGameState->Teams[2]->TeamColor);
	DrawTexture(UTHUDOwner->ScoreboardAtlas, TeamEdgeSize + FrontSize + MiddleSize, BackgroundY, EndSize, BackgroundHeight, 39, 188, 64, 65, 1.0, UTGameState->Teams[2]->TeamColor);

	DrawText(FText::AsNumber(UTGameState->Teams[2]->Score), NamePosition, TeamScoreY, UTHUDOwner->HugeFont, false, FVector2D(0, 0), FLinearColor::Black, true, FLinearColor::Black, 1.5f*RenderScale, 1.f, FLinearColor::White, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), ETextHorzPos::Right, ETextVertPos::Center);

	float LeftEdge = Canvas->ClipX - TeamEdgeSize - FrontSize - MiddleSize - EndSize;
	if (GameNumTeams == 4)
	{
		DrawTexture(UTHUDOwner->ScoreboardAtlas, LeftEdge + EndSize + MiddleSize, BackgroundY, FrontSize, BackgroundHeight, 196, 188, 36, 65, 1.f, UTGameState->Teams[3]->TeamColor);
		DrawTexture(UTHUDOwner->ScoreboardAtlas, LeftEdge + EndSize, BackgroundY, MiddleSize, BackgroundHeight, 130, 188, 64, 65, 1.f, UTGameState->Teams[3]->TeamColor);
		DrawTexture(UTHUDOwner->ScoreboardAtlas, LeftEdge, BackgroundY, EndSize, BackgroundHeight, 117, 188, 16, 65, 1.f, UTGameState->Teams[3]->TeamColor);

		DrawText(FText::AsNumber(UTGameState->Teams[3]->Score), Canvas->ClipX - NamePosition, TeamScoreY, UTHUDOwner->HugeFont, false, FVector2D(0.f, 0.f), FLinearColor::Black, true, FLinearColor::Black, 1.5f*RenderScale, 1.f, FLinearColor::White, FLinearColor(0.0f, 0.0f, 0.0f, 0.0f), ETextHorzPos::Left, ETextVertPos::Center);
	}
	YOffset += 119.f * RenderScale;
}

void UMultiTeamScoreboard::Draw4ScoreHeaders(float RenderDelta, float& YOffset)
{
	float Height = 23.f * RenderScale;
	int32 ColumnCnt = ((UTGameState && UTGameState->bTeamGame) || ActualPlayerCount > 16) ? 2 : 1;
	float ColumnHeaderAdjustY = ColumnHeaderY * RenderScale;
	float XOffset = ScaledEdgeSize;
	for (int32 i = 0; i < ColumnCnt; i++)
	{
		if (i == 1 && GameNumTeams == 3)
		{
			YOffset += Height + 4.f * RenderScale;
			return;
		}

		// Draw the background Border
		DrawTexture(UTHUDOwner->ScoreboardAtlas, XOffset, YOffset, ScaledCellWidth, Height, 149, 138, 32, 32, 1.0, FLinearColor(0.72f, 0.72f, 0.72f, 0.85f));
		DrawText(CH_PlayerName, XOffset + (ScaledCellWidth * ColumnHeaderPlayerX), YOffset + ColumnHeaderAdjustY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Left, ETextVertPos::Center);

		if (UTGameState && UTGameState->HasMatchStarted())
		{
			DrawText(CH_Score, XOffset + (ScaledCellWidth * ColumnHeaderScoreX), YOffset + ColumnHeaderAdjustY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
			DrawText(CH_Deaths, XOffset + (ScaledCellWidth * ColumnHeaderDeathsX), YOffset + ColumnHeaderAdjustY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
		}
		else
		{
			DrawText(CH_Ready, XOffset + (ScaledCellWidth * ColumnHeaderScoreX), YOffset + ColumnHeaderAdjustY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
		}
		DrawText((GetWorld()->GetNetMode() == NM_Standalone) ? CH_Skill : CH_Ping, XOffset + (ScaledCellWidth * ColumnHeaderPingX), YOffset + ColumnHeaderAdjustY, UTHUDOwner->TinyFont, RenderScale, 1.0f, FLinearColor::Black, ETextHorzPos::Center, ETextVertPos::Center);
		XOffset = Canvas->ClipX - ScaledCellWidth - ScaledEdgeSize;
	}

	YOffset += Height + 4.f * RenderScale;
}

void UMultiTeamScoreboard::Draw4PlayerScores(float RenderDelta, float& YOffset)
{
	if (UTGameState == nullptr)
	{
		return;
	}

	int32 NumSpectators = 0;
	int32 XOffset = ScaledEdgeSize;
	float MaxYOffset = 0.f;
	for (int8 Team = 2; Team < GameNumTeams; Team++)
	{
		int32 Place = 1;
		float DrawOffset = YOffset;
		int32 NumPlayersToShow = ShouldDrawScoringStats() ? 5 : UTGameState->PlayerArray.Num();
		for (int32 i = 0; i < UTGameState->PlayerArray.Num(); i++)
		{
			AUTPlayerState* PlayerState = Cast<AUTPlayerState>(UTGameState->PlayerArray[i]);
			if (PlayerState)
			{
				if (!PlayerState->bOnlySpectator)
				{
					if (PlayerState->GetTeamNum() == Team)
					{
						DrawPlayer(Place, PlayerState, RenderDelta, XOffset, DrawOffset);
						Place++;
						DrawOffset += CellHeight*RenderScale;
						if (Place > NumPlayersToShow)
						{
							break;
						}
					}
				}
				else if (Team == 0 && (Cast<AUTDemoRecSpectator>(UTPlayerOwner) == nullptr && !PlayerState->bIsDemoRecording))
				{
					NumSpectators++;
				}
			}
		}
		MaxYOffset = FMath::Max(DrawOffset, MaxYOffset);
		XOffset = Canvas->ClipX - ScaledCellWidth - ScaledEdgeSize;
	}
	YOffset = MaxYOffset;

	if ((UTGameState->PlayerArray.Num() <= 28) && (NumSpectators > 0) && !ShouldDrawScoringStats())
	{
		FText SpectatorCount = (NumSpectators == 1)
			? OneSpectatorWatchingText
			: FText::Format(SpectatorsWatchingText, FText::AsNumber(NumSpectators));
		DrawText(SpectatorCount, Size.X * 0.5f, 765.f*RenderScale, UTHUDOwner->SmallFont, 1.0f, 1.0f, FLinearColor(0.75f, 0.75f, 0.75f, 1.0f), ETextHorzPos::Center, ETextVertPos::Bottom);
	}
}

void UMultiTeamScoreboard::DrawClockTeamStatsLine(FText StatsName, FName StatsID, float DeltaTime, float XOffset, float& YPos, const FStatsFontInfo& StatsFontInfo, float ScoreWidth, bool bSkipEmpty)
{
	int32 HighlightIndex = 0;
	int32 RedTeamValue = UTGameState->Teams[0]->GetStatsValue(StatsID);
	int32 BlueTeamValue = UTGameState->Teams[1]->GetStatsValue(StatsID);
	int32 GreenTeamValue = 0;
	int32 GoldTeamValue = 0;
	if (GameNumTeams > 2)
	{
		GreenTeamValue = UTGameState->Teams[2]->GetStatsValue(StatsID);
		if (GameNumTeams == 4)
		{
			GoldTeamValue = UTGameState->Teams[3]->GetStatsValue(StatsID);
		}
	}
	if (bSkipEmpty && (RedTeamValue == 0) && (BlueTeamValue == 0) && (GreenTeamValue == 0) && (GoldTeamValue == 0))
	{
		return;
	}
	if (RedTeamValue < BlueTeamValue)
	{
		HighlightIndex = 2;
	}
	else if (RedTeamValue > BlueTeamValue)
	{
		HighlightIndex = 1;
	}

	FText ClockStringRed = UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), RedTeamValue, false);
	FText ClockStringBlue = UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), BlueTeamValue, false);
	FText ClockStringGreen = FText::GetEmpty();
	FText ClockStringGold = FText::GetEmpty();
	if (GameNumTeams > 2)
	{
		ClockStringGreen = UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), GreenTeamValue, false);
		if (GameNumTeams == 4)
		{
			ClockStringGold = UTHUDOwner->ConvertTime(FText::GetEmpty(), FText::GetEmpty(), GoldTeamValue, false);
		}
	}

	DrawTextStatsLine(StatsName, ClockStringRed.ToString(), ClockStringBlue.ToString(), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, HighlightIndex);

	if (GreenTeamValue < GoldTeamValue)
	{
		HighlightIndex = 3;
	}
	else if (GreenTeamValue > GoldTeamValue)
	{
		HighlightIndex = 4;
	}

	// TEMP
	float OtherTeamPosY = YPos + 40.f * RenderScale;
	if (GameNumTeams > 2)
	{
		DrawTextStatsLine(StatsName, ClockStringGreen.ToString(), ClockStringGold.ToString(), DeltaTime, XOffset, OtherTeamPosY, StatsFontInfo, ScoreWidth, HighlightIndex);
	}
}

void UMultiTeamScoreboard::Draw4TeamScoreBreakdown(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom)
{
	Canvas->SetLinearDrawColor(FLinearColor::White);
	FStatsFontInfo StatsFontInfo;
	StatsFontInfo.TextRenderInfo.bEnableShadow = true;
	StatsFontInfo.TextRenderInfo.bClipText = true;
	StatsFontInfo.TextFont = UTHUDOwner->TinyFont;
	bHighlightStatsLineTopValue = true;

	float XL, SmallYL;
	Canvas->TextSize(UTHUDOwner->TinyFont, "TEST", XL, SmallYL, RenderScale, RenderScale);
	StatsFontInfo.TextHeight = SmallYL;
	float MedYL;
	Canvas->TextSize(UTHUDOwner->SmallFont, TeamScoringHeader.ToString(), XL, MedYL, RenderScale, RenderScale);
	Canvas->DrawText(UTHUDOwner->SmallFont, TeamScoringHeader, XOffset + 0.5f*(ScoreWidth - XL), YPos, RenderScale, RenderScale, StatsFontInfo.TextRenderInfo);
	YPos += 1.1f * MedYL;

	if (UTGameState == NULL || UTGameState->Teams.Num() < 2 || (UTGameState->NumTeams > 2 && UTGameState->Teams[2] == NULL) || (UTGameState->NumTeams == 4 && UTGameState->Teams[3] == NULL))
	{
		return;
	}

	// draw team icons
	float IconHeight = MedYL;
	DrawTexture(UTHUDOwner->HUDAtlas, XOffset + ValueColumn*ScoreWidth - IconHeight, YPos, IconHeight, IconHeight, UTHUDOwner->TeamIconUV[2].X, UTHUDOwner->TeamIconUV[2].Y, 72, 72, 1.f, UTGameState->Teams[2]->TeamColor);
	if (GameNumTeams == 4)
	{
		DrawTexture(UTHUDOwner->HUDAtlas, XOffset + ScoreColumn*ScoreWidth - IconHeight, YPos, IconHeight, IconHeight, UTHUDOwner->TeamIconUV[3].X, UTHUDOwner->TeamIconUV[3].Y, 72, 72, 1.f, UTGameState->Teams[3]->TeamColor);
	}
	Canvas->DrawText(UTHUDOwner->MediumFont, FText::AsNumber(UTGameState->Teams[2]->Score), XOffset + ValueColumn*ScoreWidth, YPos - 0.5f * MedYL, RenderScale, RenderScale, StatsFontInfo.TextRenderInfo);
	if (GameNumTeams == 4)
	{
		Canvas->DrawText(UTHUDOwner->MediumFont, FText::AsNumber(UTGameState->Teams[3]->Score), XOffset + ScoreColumn*ScoreWidth, YPos - 0.5f * MedYL, RenderScale, RenderScale, StatsFontInfo.TextRenderInfo);
	}
	YPos += 1.5f * MedYL;
	Draw4TeamStats(DeltaTime, YPos, XOffset, ScoreWidth, PageBottom, StatsFontInfo);
}

void UMultiTeamScoreboard::DrawTeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo)
{
	// draw team stats
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "TeamKills", "Kills"), UTGameState->Teams[0]->GetStatsValue(NAME_TeamKills), UTGameState->Teams[1]->GetStatsValue(NAME_TeamKills), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);

	// find top scorer
	AUTPlayerState* TopScorerRed = FindTopTeamScoreFor(0);
	AUTPlayerState* TopScorerBlue = FindTopTeamScoreFor(1);
	AUTPlayerState* TopTeamCapRed = FindTopTeamScoreFor(0);
	AUTPlayerState* TopTeamCapBlue = FindTopTeamScoreFor(1);

	// find top kills && KD
	AUTPlayerState* TopKillerRed = FindTopTeamKillerFor(0);
	AUTPlayerState* TopKillerBlue = FindTopTeamKillerFor(1);
	AUTPlayerState* TopKDRed = FindTopTeamKDFor(0);
	AUTPlayerState* TopKDBlue = FindTopTeamKDFor(1);
	AUTPlayerState* TopSPMRed = FindTopTeamSPMFor(0);
	AUTPlayerState* TopSPMBlue = FindTopTeamSPMFor(1);

	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopScorer", "Top Scorer"), TopScorerRed, TopScorerBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawPlayerStatsLine(NSLOCTEXT("MultiTeamScoreboard", "TopCaps", "Top Conrtol Point Captures"), TopTeamCapRed, TopTeamCapBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopKD", "Top K/D"), TopKDRed, TopKDBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopSPM", "Top Score Per Minute"), TopSPMRed, TopSPMBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	YPos += StatsFontInfo.TextHeight;

	DrawStatsLine(NSLOCTEXT("UTScoreboard", "BeltPickups", "Shield Belt Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ShieldBeltCount), UTGameState->Teams[1]->GetStatsValue(NAME_ShieldBeltCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "VestPickups", "Armor Vest Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ArmorVestCount), UTGameState->Teams[1]->GetStatsValue(NAME_ArmorVestCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "PadPickups", "Thigh Pad Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_ArmorPadsCount), UTGameState->Teams[1]->GetStatsValue(NAME_ArmorPadsCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	if (UTGameState->Teams[0]->GetStatsValue(NAME_HelmetCount) + UTGameState->Teams[1]->GetStatsValue(NAME_HelmetCount) > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "HelmetPickups", "Helmet Pickups"), UTGameState->Teams[0]->GetStatsValue(NAME_HelmetCount), UTGameState->Teams[1]->GetStatsValue(NAME_HelmetCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	int32 TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_UDamageCount);
	int32 TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_UDamageCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "UDamagePickups", "UDamage Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_BerserkCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_BerserkCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "BerserkPickups", "Berserk Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_InvisibilityCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_InvisibilityCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "InvisibilityPickups", "Invisibility Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[0]->GetStatsValue(NAME_KegCount);
	TeamStat1 = UTGameState->Teams[1]->GetStatsValue(NAME_KegCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "KegPickups", "Keg Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}

	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "UDamage", "UDamage Control"), NAME_UDamageTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Berserk", "Berserk Control"), NAME_BerserkTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Invisibility", "Invisibility Control"), NAME_InvisibilityTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);

	int32 BootJumpsRed = UTGameState->Teams[0]->GetStatsValue(NAME_BootJumps);
	int32 BootJumpsBlue = UTGameState->Teams[1]->GetStatsValue(NAME_BootJumps);
	if ((BootJumpsRed != 0) || (BootJumpsBlue != 0))
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "JumpBootJumps", "JumpBoot Jumps"), BootJumpsRed, BootJumpsBlue, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}

	// @TOD FIXMESTEVE redeemer shots as part of map control
	// make all the loc text into properties instead of recalc
}

void UMultiTeamScoreboard::Draw4TeamStats(float DeltaTime, float& YPos, float XOffset, float ScoreWidth, float PageBottom, const FStatsFontInfo& StatsFontInfo)
{
	// draw team stats
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "TeamKills", "Kills"), UTGameState->Teams[2]->GetStatsValue(NAME_TeamKills), UTGameState->Teams[3]->GetStatsValue(NAME_TeamKills), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);

	// find top scorer
	AUTPlayerState* TopScorerGreen = FindTopTeamScoreFor(2);
	AUTPlayerState* TopScorerGold = FindTopTeamScoreFor(3);
	AUTPlayerState* TopTeamCapGreen = FindTopTeamScoreFor(2);
	AUTPlayerState* TopTeamCapGold = FindTopTeamScoreFor(3);

	// find top kills && KD
	AUTPlayerState* TopKillerGreen = FindTopTeamKillerFor(2);
	AUTPlayerState* TopKillerGold = FindTopTeamKillerFor(3);
	AUTPlayerState* TopKDGreen = FindTopTeamKDFor(2);
	AUTPlayerState* TopKDGold = FindTopTeamKDFor(3);
	AUTPlayerState* TopSPMGreen = FindTopTeamSPMFor(2);
	AUTPlayerState* TopSPMGold = FindTopTeamSPMFor(3);

	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopScorer", "Top Scorer"), TopScorerGreen, TopScorerGold, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawPlayerStatsLine(NSLOCTEXT("MultiTeamScoreboard", "TopCaps", "Top Conrtol Point Captures"), TopTeamCapGreen, TopTeamCapGold, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopKD", "Top K/D"), TopKDGreen, TopKDGold, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	DrawPlayerStatsLine(NSLOCTEXT("UTScoreboard", "TopSPM", "Top Score Per Minute"), TopSPMGreen, TopSPMGold, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, 0);
	YPos += StatsFontInfo.TextHeight;

	DrawStatsLine(NSLOCTEXT("UTScoreboard", "BeltPickups", "Shield Belt Pickups"), UTGameState->Teams[2]->GetStatsValue(NAME_ShieldBeltCount), UTGameState->Teams[3]->GetStatsValue(NAME_ShieldBeltCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "VestPickups", "Armor Vest Pickups"), UTGameState->Teams[2]->GetStatsValue(NAME_ArmorVestCount), UTGameState->Teams[3]->GetStatsValue(NAME_ArmorVestCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	DrawStatsLine(NSLOCTEXT("UTScoreboard", "PadPickups", "Thigh Pad Pickups"), UTGameState->Teams[2]->GetStatsValue(NAME_ArmorPadsCount), UTGameState->Teams[3]->GetStatsValue(NAME_ArmorPadsCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	if (UTGameState->Teams[2]->GetStatsValue(NAME_HelmetCount) + UTGameState->Teams[3]->GetStatsValue(NAME_HelmetCount) > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "HelmetPickups", "Helmet Pickups"), UTGameState->Teams[2]->GetStatsValue(NAME_HelmetCount), UTGameState->Teams[3]->GetStatsValue(NAME_HelmetCount), DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	int32 TeamStat0 = UTGameState->Teams[2]->GetStatsValue(NAME_UDamageCount);
	int32 TeamStat1 = UTGameState->Teams[3]->GetStatsValue(NAME_UDamageCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "UDamagePickups", "UDamage Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[2]->GetStatsValue(NAME_BerserkCount);
	TeamStat1 = UTGameState->Teams[3]->GetStatsValue(NAME_BerserkCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "BerserkPickups", "Berserk Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[2]->GetStatsValue(NAME_InvisibilityCount);
	TeamStat1 = UTGameState->Teams[3]->GetStatsValue(NAME_InvisibilityCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "InvisibilityPickups", "Invisibility Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}
	TeamStat0 = UTGameState->Teams[2]->GetStatsValue(NAME_KegCount);
	TeamStat1 = UTGameState->Teams[3]->GetStatsValue(NAME_KegCount);
	if (TeamStat0 > 0 || TeamStat1 > 0)
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "KegPickups", "Keg Pickups"), TeamStat0, TeamStat1, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}

	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "UDamage", "UDamage Control"), NAME_UDamageTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Berserk", "Berserk Control"), NAME_BerserkTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);
	DrawClockTeamStatsLine(NSLOCTEXT("UTScoreboard", "Invisibility", "Invisibility Control"), NAME_InvisibilityTime, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth, true);

	int32 BootJumpsGreen = UTGameState->Teams[2]->GetStatsValue(NAME_BootJumps);
	int32 BootJumpsGold = UTGameState->Teams[3]->GetStatsValue(NAME_BootJumps);
	if ((BootJumpsGreen != 0) || (BootJumpsGold != 0))
	{
		DrawStatsLine(NSLOCTEXT("UTScoreboard", "JumpBootJumps", "JumpBoot Jumps"), BootJumpsGreen, BootJumpsGold, DeltaTime, XOffset, YPos, StatsFontInfo, ScoreWidth);
	}

	// @TOD FIXMESTEVE redeemer shots as part of map control
	// make all the loc text into properties instead of recalc
}
