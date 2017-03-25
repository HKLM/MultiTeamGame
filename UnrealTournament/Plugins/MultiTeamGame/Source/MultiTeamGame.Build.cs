// Created by Brian 'Snake' Alexander, 2017
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class MultiTeamGame : ModuleRules
    {
        public MultiTeamGame(TargetInfo Target)
        {
            PrivateIncludePaths.Add("MultiTeamGame/Private");
            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Navmesh",
                    "UnrealTournament",
                    "InputCore",
                    "Slate",
                    "SlateCore"
                }
                );
        }
    }
}