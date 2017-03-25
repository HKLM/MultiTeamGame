// Created by Brian 'Snake' Alexander, 2017
#include "UnrealTournament.h"
#include "MultiTeamGameMode.h"
#include "Core.h"
#include "Engine.h"
#include "ModuleInterface.h"
#include "ModuleManager.h"

class MULTITEAMGAME_API FMultiTeamGamePlugin : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FMultiTeamGamePlugin, MultiTeamGame )

void FMultiTeamGamePlugin::StartupModule()
{}

void FMultiTeamGamePlugin::ShutdownModule()
{}
