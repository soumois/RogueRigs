// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RogueRigs : ModuleRules
{
	public RogueRigs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Json",
			"JsonUtilities"
		});

		PublicIncludePaths.AddRange(new string[] {
			"RogueRigs",
			"RogueRigs/Variant_Strategy",
			"RogueRigs/Variant_Strategy/UI",
			"RogueRigs/Variant_TwinStick",
			"RogueRigs/Variant_TwinStick/AI",
			"RogueRigs/Variant_TwinStick/Gameplay",
			"RogueRigs/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
