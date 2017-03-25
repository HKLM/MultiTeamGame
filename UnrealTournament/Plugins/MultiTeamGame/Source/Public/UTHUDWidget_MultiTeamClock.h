/**
 * Created by Brian 'Snake' Alexander, 2017
 * Adds 4 team support by drawing the Team Scores and game clock on the Top part of the HUD
 */
#pragma once

#include "UnrealTournament.h"
#include "UTHUDWidget_MultiTeamClock.generated.h"

UCLASS()
class MULTITEAMGAME_API UUTHUDWidget_MultiTeamClock : public UUTHUDWidget
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, Category = "Scoreboard")
	UTexture2D* MTAtlas;

	/* array of team icon HUD textures */
	UPROPERTY(EditAnywhere, Category = "Scoreboard")
	TArray<UTexture2D*> MultiTeamIcon;

	virtual void InitializeWidget(AUTHUD* Hud);
	virtual void Draw_Implementation(float DeltaTime);

	virtual bool ShouldDraw_Implementation(bool bShowScores)
	{
		return !bShowScores;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Texture BackgroundSlate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Texture BackgroundBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Texture RedTeamLogo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Texture BlueTeamLogo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Texture GreenTeamLogo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Texture GoldTeamLogo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text RedScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text BlueScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text GreenScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text GoldScoreText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text RoleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text TeamNameText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text ClockText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FHUDRenderObject_Text GameStateText;

	// The scale factor to use on the clock when it has to show hours
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	float AltClockScale;

	UFUNCTION(BlueprintNativeEvent, Category = "RenderObject")
	FText GetRedScoreText();

	UFUNCTION(BlueprintNativeEvent, Category = "RenderObject")
	FText GetBlueScoreText();

	UFUNCTION(BlueprintNativeEvent, Category = "RenderObject")
	FText GetGreenScoreText();

	UFUNCTION(BlueprintNativeEvent, Category = "RenderObject")
	FText GetGoldScoreText();

	UFUNCTION(BlueprintNativeEvent, Category = "RenderObject")
	FText GetClockText();
};