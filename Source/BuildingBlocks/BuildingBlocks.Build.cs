// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BuildingBlocks : ModuleRules
{
	public BuildingBlocks(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
