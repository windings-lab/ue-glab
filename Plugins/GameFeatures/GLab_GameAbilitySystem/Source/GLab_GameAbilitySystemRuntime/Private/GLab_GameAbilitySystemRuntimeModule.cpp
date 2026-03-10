// Copyright Epic Games, Inc. All Rights Reserved.

#include "GLab_GameAbilitySystemRuntimeModule.h"

#define LOCTEXT_NAMESPACE "FGLab_GameAbilitySystemRuntimeModule"

void FGLab_GameAbilitySystemRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory;
	// the exact timing is specified in the .uplugin file per-module
}

void FGLab_GameAbilitySystemRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.
	// For modules that support dynamic reloading, we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGLab_GameAbilitySystemRuntimeModule, GLab_GameAbilitySystemRuntime)
