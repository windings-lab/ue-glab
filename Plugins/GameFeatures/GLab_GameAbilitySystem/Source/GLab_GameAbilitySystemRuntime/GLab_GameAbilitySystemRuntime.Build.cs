// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GLab_GameAbilitySystemRuntime : ModuleRules
{
	public GLab_GameAbilitySystemRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{

			}
		);
	}
}
