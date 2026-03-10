// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GLab_GASBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GLAB_GAMEABILITYSYSTEMRUNTIME_API UGLab_GASBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = Ability)
	static void AddGameplayTag(
		UAbilitySystemComponent* ASC,
		FGameplayTag GameplayTag,
		int32 Count = 1,
		EGameplayTagReplicationState TagRepState = EGameplayTagReplicationState::None);

	UFUNCTION(BlueprintCallable, Category = Ability)
	static void RemoveGameplayTag(
		UAbilitySystemComponent* ASC,
		FGameplayTag GameplayTag,
		int32 Count = 1,
		EGameplayTagReplicationState TagRepState = EGameplayTagReplicationState::None);
};
