// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SJGame : ModuleRules
{
	public SJGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"SJGame"
			}
		);
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"ApplicationCore",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"ModularGameplay",
			"ModularGameplayActors",
			"GameplayTags",
			"GameplayAbilities",
			"GameplayTasks",
			"GameFeatures",
			"ModularGameplay",
			"ModularGameplayActors"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"DeveloperSettings",
			"CommonGame",
			"CommonUser"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
