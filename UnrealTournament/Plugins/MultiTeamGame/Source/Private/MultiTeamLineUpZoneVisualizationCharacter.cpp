// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "UTLineUpZoneVisualizationCharacter.h"
#include "MultiTeamGameState.h"
#include "MultiTeamSkinName.h"
#include "MultiTeamLineUpZoneVisualizationCharacter.h"

AMultiTeamLineUpZoneVisualizationCharacter::AMultiTeamLineUpZoneVisualizationCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AMultiTeamLineUpZoneVisualizationCharacter::OnChangeTeamNum()
{
	DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
	/* value what base color to use (red or blue) */
	float FTeamNum = (TeamNum == 1 || TeamNum == 2) ? 1.0f : 0.0f;
	AMultiTeamGameState* MTGS = Cast<AMultiTeamGameState>(GetWorld()->GetGameState());
	if (MTGS)
	{
		FLinearColor TeamBodySkinColor = MTGS->TeamBodySkinColor[TeamNum];
		FLinearColor TeamSkinOverlayColor = MTGS->TeamSkinOverlayColor[TeamNum];

		DynMaterial->SetScalarParameterValue(NAME_UseTeamColors, 1.0f);
		DynMaterial->SetScalarParameterValue(NAME_TeamSelect, FTeamNum);
		if (TeamNum == 1 || TeamNum == 2) //Blue & Green Team
		{
			// We use blue team base skin also for the green team
			DynMaterial->SetVectorParameterValue(NAME_TeamBlueColor, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_TeamBlueOverlay, TeamSkinOverlayColor);
			//merc
			DynMaterial->SetVectorParameterValue(NAME_MercMaleTeamBluePlastic, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_MercMaleTeamBlueCamo, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_MercMaleTeamBlueNylon, TeamBodySkinColor);
			//nec male
			DynMaterial->SetVectorParameterValue(NAME_NecMaleTeamBlueDarkPlastic, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_NecMaleTeamBlueLatex, TeamBodySkinColor);
			//visse
			DynMaterial->SetVectorParameterValue(NAME_VisseTeamRubberTint, TeamBodySkinColor);
			//Skaarj
			DynMaterial->SetVectorParameterValue(NAME_SkaarjTeamBlueTintAlpha, TeamBodySkinColor);
		}
		else if (TeamNum == 0 || TeamNum == 3) //Red & Gold Team
		{
			// We use red team base skin also for the gold team, because there is no blue in the color yellow
			DynMaterial->SetVectorParameterValue(NAME_TeamRedColor, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_TeamRedOverlay, TeamSkinOverlayColor);
			//merc
			DynMaterial->SetVectorParameterValue(NAME_MercMaleTeamRedPlastic, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_MercMaleTeamRedCamo, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_MercMaleTeamRedNylon, TeamBodySkinColor);
			//nec male
			DynMaterial->SetVectorParameterValue(NAME_NecMaleTeamRedDarkPlastic, TeamBodySkinColor);
			DynMaterial->SetVectorParameterValue(NAME_NecMaleTeamRedLatex, TeamBodySkinColor);
			//visse
			DynMaterial->SetVectorParameterValue(NAME_VisseTeamRubberTint, TeamBodySkinColor);
			//Skaarj
			DynMaterial->SetVectorParameterValue(NAME_SkaarjTeamRedTintAlpha, TeamBodySkinColor);
		}

		GetMesh()->SetMaterial(0, DynMaterial);
	}
}
