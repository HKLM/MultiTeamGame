// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "MultiTeamGameState.h"
#include "UTHUD_MultiTeam.h"

AUTHUD_MultiTeam::AUTHUD_MultiTeam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> DHUDTex(TEXT("Texture'/MultiTeamGame/MultiTeamGameContent/Textures/MTHUDAtlas01.MTHUDAtlas01'"));
	HUDMultiTeamAtlas = DHUDTex.Object;

	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Paperdoll.bpHW_Paperdoll_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponInfo.bpHW_WeaponInfo_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/MultiTeamGame.UTHUDWidget_MultiTeamClock"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_WeaponBar.bpHW_WeaponBar_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_Powerups.bpHW_Powerups_C"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_FloatingScore.bpHW_FloatingScore_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidget_WeaponCrosshair"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetMessage_ConsoleMessages"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidgetAnnouncements"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpWH_KillIconMessages.bpWH_KillIconMessages_C"));
	HudWidgetClasses.AddUnique(TEXT("/Script/UnrealTournament.UTHUDWidget_Spectator"));
	HudWidgetClasses.AddUnique(TEXT("/Script/MultiTeamGame.MultiTeamScoreboard"));
	HudWidgetClasses.AddUnique(TEXT("/Game/RestrictedAssets/UI/HUDWidgets/bpHW_QuickStats.bpHW_QuickStats_C"));

	MultiTeamIconUV[0] = FVector2D(257.f, 940.f);
	MultiTeamIconUV[1] = FVector2D(333.f, 940.f);
	MultiTeamIconUV[2] = FVector2D(257.f, 853.f);
	MultiTeamIconUV[3] = FVector2D(333.f, 853.f);
	MultiTeamIconUV[4] = FVector2D(256.f, 770.f);
}

void AUTHUD_MultiTeam::BeginPlay()
{
	HUDAtlas = HUDMultiTeamAtlas;
	Super::BeginPlay();
}

FLinearColor AUTHUD_MultiTeam::GetBaseHUDColor()
{
	FLinearColor TeamColor = Super::GetBaseHUDColor();
	APawn* HUDPawn = Cast<APawn>(UTPlayerOwner->GetViewTarget());
	if (HUDPawn)
	{
		AUTPlayerState* PS = Cast<AUTPlayerState>(HUDPawn->PlayerState);
		if (PS != nullptr && PS->Team != nullptr)
		{
			TeamColor = PS->Team->TeamColor;
		}
	}
	return TeamColor;
}

FLinearColor AUTHUD_MultiTeam::GetWidgetTeamColor()
{
	// Add code to cache and return the team color if it's a team game
	AUTGameState* GS = GetWorld()->GetGameState<AUTGameState>();
	if (GS == nullptr || (GS->bTeamGame && UTPlayerOwner && UTPlayerOwner->GetViewTarget()))
	{
		APawn* HUDPawn = Cast<APawn>(UTPlayerOwner->GetViewTarget());
		AUTPlayerState* PS = HUDPawn ? Cast<AUTPlayerState>(HUDPawn->PlayerState) : NULL;
		if (PS != nullptr)
		{
			switch (PS->GetTeamNum())
			{
				case 0: return FLinearColor(0.15, 0.0, 0.0, 1.0); break;
				case 1: return FLinearColor(0.025, 0.025, 0.1, 1.0); break;
				case 2: return FLinearColor(0.0, 0.25, 0.0, 1.0); break;
				case 3: return FLinearColor(0.25, 0.25, 0.0, 1.0); break;
				default: break;
			}
		}
	}

	return FLinearColor::Black;
}

FColor AUTHUD_MultiTeam::GetColorByTeamIndex(int32 TeamIndex)
{
	switch (TeamIndex)
	{
		case 0: return FColor(255, 0, 0, 255); break;
		case 1: return FColor(0, 0, 255, 255); break;
		case 2: return FColor(0, 255, 0, 255); break;
		case 3: return FColor(255, 255, 0, 255); break;
		case 5: return FColor(0, 0, 0, 255); break;
		default: return FColor(155, 155, 155, 255); break;
	}
}

FLinearColor AUTHUD_MultiTeam::GetLinearColorByTeamIndex(int32 TeamIndex)
{
	switch (TeamIndex)
	{
		case 0: return FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); break;
		case 1: return FLinearColor(0.025f, 0.025f, 1.0f, 1.0f); break;
		case 2: return FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); break;
		case 3: return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); break;
		case 4: return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); break;
		default: return FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); break;
	}
}
