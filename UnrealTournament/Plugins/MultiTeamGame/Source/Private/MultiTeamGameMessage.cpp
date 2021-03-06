// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTLocalMessage.h"
#include "UTGameMessage.h"
#include "MultiTeamGameMessage.h"

UMultiTeamGameMessage::UMultiTeamGameMessage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MessageArea = FName(TEXT("Announcements"));
	MessageSlot = FName(TEXT("GameMessages"));
\
	Lifetime = 1.2;
	bIsStatusAnnouncement = true;
	bIsPartiallyUnique = true;
}

FLinearColor UMultiTeamGameMessage::GetMessageColor_Implementation(int32 MessageIndex) const
{
	switch (MessageIndex)
	{
		case 0: return FLinearColor(0.8f, 0.0f, 0.05f, 1.0f); break;
		case 1: return FLinearColor(0.1f, 0.1f, 0.8f, 1.0f); break;
		case 2: return FLinearColor(0.0f, 0.9f, 0.0f, 1.0f); break;
		case 3: return FLinearColor(0.85f, 0.85f, 0.0f, 1.0f); break;
		case 9: return FLinearColor(0.8f, 0.0f, 0.05f, 1.0f); break;
		case 10: return FLinearColor(0.1f, 0.1f, 0.8f, 1.0f); break;
		case 11: return FLinearColor(0.0f, 0.9f, 0.0f, 1.0f); break;
		case 12: return FLinearColor(0.85f, 0.85f, 0.0f, 1.0f); break;
		default:
			break;
	}
	return FLinearColor::Yellow;
}

FText UMultiTeamGameMessage::GetText(int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const
{
	switch (Switch)
	{
		case 9: return BuildEmphasisText(Switch, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject);
		case 10: return BuildEmphasisText(Switch, RelatedPlayerState_1, RelatedPlayerState_2, OptionalObject);
		//case 11: return YouAreOnGreen; break;
		//case 12: return YouAreOnGold; break;

		default:
			break;
	}

	return FText::GetEmpty();
}

void UMultiTeamGameMessage::GetArgs(FFormatNamedArguments& Args, int32 Switch, bool bTargetsPlayerState1, APlayerState* RelatedPlayerState_1, APlayerState* RelatedPlayerState_2, UObject* OptionalObject) const
{
	Args.Add(TEXT("Player1Name"), RelatedPlayerState_1 != NULL ? FText::FromString(RelatedPlayerState_1->PlayerName) : FText::GetEmpty());
	Args.Add(TEXT("Player1Score"), RelatedPlayerState_1 != NULL ? FText::AsNumber(int32(RelatedPlayerState_1->Score)) : FText::GetEmpty());
	Args.Add(TEXT("Player2Name"), RelatedPlayerState_2 != NULL ? FText::FromString(RelatedPlayerState_2->PlayerName) : FText::GetEmpty());
	Args.Add(TEXT("Player2Score"), RelatedPlayerState_2 != NULL ? FText::AsNumber(int32(RelatedPlayerState_2->Score)) : FText::GetEmpty());

	AUTPlayerState* UTPlayerState = Cast<AUTPlayerState>(RelatedPlayerState_1);
	Args.Add(TEXT("Player1Team"), (UTPlayerState && UTPlayerState->Team) ? UTPlayerState->Team->TeamName : FText::GetEmpty());

	UTPlayerState = Cast<AUTPlayerState>(RelatedPlayerState_2);
	Args.Add(TEXT("Player2Team"), (UTPlayerState && UTPlayerState->Team) ? UTPlayerState->Team->TeamName : FText::GetEmpty());

	UClass* DamageTypeClass = Cast<UClass>(OptionalObject);
	if (DamageTypeClass != NULL && DamageTypeClass->IsChildOf(UUTDamageType::StaticClass()))
	{
		UUTDamageType* DamageType = DamageTypeClass->GetDefaultObject<UUTDamageType>();
		if (DamageType != NULL)
		{
			Args.Add(TEXT("WeaponName"), DamageType->AssociatedWeaponName);
		}
	}
		AUTTeamInfo* TeamInfo = Cast<AUTTeamInfo>(OptionalObject);
		Args.Add(TEXT("OptionalTeam"), TeamInfo ? TeamInfo->TeamName : FText::GetEmpty());
}

bool UMultiTeamGameMessage::InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UUTLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const
{
	if (Cast<UUTLocalMessage>(OtherMessageClass->GetDefaultObject())->bOptionalSpoken)
	{
		return true;
	}
	if (GetClass() == OtherMessageClass)
	{
		if (OptionalObject == OtherOptionalObject)
		{
			return true;
		}
	}
	return false;
}

FName UMultiTeamGameMessage::GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const
{
	//TODO is this really needed????
	switch (Switch)
	{
		case 9: return TEXT("YouAreOnRedTeam"); break;
		case 10: return TEXT("YouAreOnBlueTeam"); break;
	}
	return NAME_None;
}

float UMultiTeamGameMessage::GetScaleInTime_Implementation(int32 MessageIndex) const
{
	return 0.15f;
}

float UMultiTeamGameMessage::GetScaleInSize_Implementation(int32 MessageIndex) const
{
	return 0.55f;
}

int32 UMultiTeamGameMessage::GetFontSizeIndex(int32 MessageIndex, bool bTargetsLocalPlayer) const
{
	if ((MessageIndex == 0) || (MessageIndex == 1) || (MessageIndex == 2) || (MessageIndex == 3))
	{
		return 2;
	}
	else
	{
	//if ((MessageIndex == 11) || (MessageIndex == 12) || (MessageIndex == 7) || (MessageIndex == 9) || (MessageIndex == 10))
		return 1;
	}
}
