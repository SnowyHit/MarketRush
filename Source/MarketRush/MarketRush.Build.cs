// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MarketRush : ModuleRules
{
	public MarketRush(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
