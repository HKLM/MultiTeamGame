// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "MultiTeamSkinName.h"
#include "UnrealNetwork.h"
#include "UTWorldSettings.h"
#include "UTCharacter.h"
#include "MultiTeamCharacter.h"

AMultiTeamCharacter::AMultiTeamCharacter(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void AMultiTeamCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(AMultiTeamCharacter, TeamBodySkinColor, COND_None);
	DOREPLIFETIME_CONDITION(AMultiTeamCharacter, TeamSkinOverlayColor, COND_None);
}

void AMultiTeamCharacter::SetTeamSkin(uint8 NewTeamIndex)
{
	if (NewTeamIndex < 4)
	{
		/* value what base color to use (red or blue) */
		float FTeamNum = (NewTeamIndex == 1 || NewTeamIndex == 2) ? 1.0f : 0.0f;
		for (UMaterialInstanceDynamic* MIDSkin : GetBodyMIs())
		{
			if (MIDSkin)
			{
				MIDSkin->SetScalarParameterValue(NAME_UseTeamColors, 1.0f);
				MIDSkin->SetScalarParameterValue(NAME_TeamSelect, FTeamNum);
				if (NewTeamIndex == 1 || NewTeamIndex == 2) //Blue & Green Team
				{
					// We use blue team base skin also for the green team
					MIDSkin->SetVectorParameterValue(NAME_TeamBlueColor, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_TeamBlueOverlay, TeamSkinOverlayColor);
					//merc
					MIDSkin->SetVectorParameterValue(NAME_MercMaleTeamBluePlastic, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_MercMaleTeamBlueCamo, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_MercMaleTeamBlueNylon, TeamBodySkinColor);
					//nec male
					MIDSkin->SetVectorParameterValue(NAME_NecMaleTeamBlueDarkPlastic, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_NecMaleTeamBlueLatex, TeamBodySkinColor);
					//visse
					MIDSkin->SetVectorParameterValue(NAME_VisseTeamRubberTint, TeamBodySkinColor);
					//Skaarj
					MIDSkin->SetVectorParameterValue(NAME_SkaarjTeamBlueTintAlpha, TeamBodySkinColor);
				}
				else if (NewTeamIndex == 0 || NewTeamIndex == 3) //Red & Gold Team
				{
					// We use red team base skin also for the gold team, because there is no blue in the color yellow
					MIDSkin->SetVectorParameterValue(NAME_TeamRedColor, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_TeamRedOverlay, TeamSkinOverlayColor);
					//merc
					MIDSkin->SetVectorParameterValue(NAME_MercMaleTeamRedPlastic, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_MercMaleTeamRedCamo, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_MercMaleTeamRedNylon, TeamBodySkinColor);
					//nec male
					MIDSkin->SetVectorParameterValue(NAME_NecMaleTeamRedDarkPlastic, TeamBodySkinColor);
					MIDSkin->SetVectorParameterValue(NAME_NecMaleTeamRedLatex, TeamBodySkinColor);
					//visse
					MIDSkin->SetVectorParameterValue(NAME_VisseTeamRubberTint, TeamBodySkinColor);
					//Skaarj
					MIDSkin->SetVectorParameterValue(NAME_SkaarjTeamRedTintAlpha, TeamBodySkinColor);
				}
			}
		}
	}
}
