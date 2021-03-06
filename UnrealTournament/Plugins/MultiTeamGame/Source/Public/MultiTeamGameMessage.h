// Created by Brian 'Snake' Alexander, 2017
#pragma once

#include "UnrealTournament.h"
#include "UTGameMessage.h"
#include "MultiTeamGameMessage.generated.h"

UCLASS(NotPlaceable)
class MULTITEAMGAME_API UMultiTeamGameMessage : public UUTGameMessage 
{
	GENERATED_UCLASS_BODY()

	virtual bool InterruptAnnouncement_Implementation(int32 Switch, const UObject* OptionalObject, TSubclassOf<UUTLocalMessage> OtherMessageClass, int32 OtherSwitch, const UObject* OtherOptionalObject) const override;
	virtual FText GetText(int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const override;
	virtual void GetArgs(FFormatNamedArguments& Args, int32 Switch = 0, bool bTargetsPlayerState1 = false, class APlayerState* RelatedPlayerState_1 = NULL, class APlayerState* RelatedPlayerState_2 = NULL, class UObject* OptionalObject = NULL) const override;
	virtual FLinearColor GetMessageColor_Implementation(int32 MessageIndex) const override;
	virtual float GetScaleInTime_Implementation(int32 MessageIndex) const override;
	virtual float GetScaleInSize_Implementation(int32 MessageIndex) const override;
	virtual FName GetAnnouncementName_Implementation(int32 Switch, const UObject* OptionalObject, const class APlayerState* RelatedPlayerState_1, const class APlayerState* RelatedPlayerState_2) const override;
	virtual int32 GetFontSizeIndex(int32 MessageIndex, bool bTargetsLocalPlayer) const override;
};
