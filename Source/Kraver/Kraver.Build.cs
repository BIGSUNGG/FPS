// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Kraver : ModuleRules
{
	public Kraver(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "Niagara", "MultiplayerSessions", "OnlineSubsystem", "OnlineSubsystemSteam", "SlateCore", "GameplayTasks", "NavigationSystem", "AdvancedWidgets", "LevelSequence", "MovieScene" });
        PrivateDependencyModuleNames.AddRange(new string[] {  });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
