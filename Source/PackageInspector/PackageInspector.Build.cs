// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PackageInspector : ModuleRules
{
	public PackageInspector(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		if (Target.bBuildEditor)
		{
			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
			);
				
		
			PrivateIncludePaths.AddRange(
				new string[] {
					// ... add other private include paths required here ...
				}
			);
			
		
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"Slate",
					"SlateCore",
					"InputCore"
				}
			);
			
		
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"UnrealEd",
					"ToolMenus",
					"ContentBrowser",
					"ApplicationCore"
				}
			);
		
		
			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
			);
		}
	}
}
