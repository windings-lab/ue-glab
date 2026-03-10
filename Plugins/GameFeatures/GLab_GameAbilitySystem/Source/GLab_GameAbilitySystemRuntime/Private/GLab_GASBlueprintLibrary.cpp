#include "GLab_GASBlueprintLibrary.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UGLab_GASBlueprintLibrary::AddGameplayTag(
	UAbilitySystemComponent* ASC,
	FGameplayTag GameplayTag,
	int32 Count,
	EGameplayTagReplicationState TagRepState)
{
	ASC->AddLooseGameplayTag(GameplayTag, Count, TagRepState);
}

void UGLab_GASBlueprintLibrary::RemoveGameplayTag(
	UAbilitySystemComponent* ASC,
	FGameplayTag GameplayTag,
	int32 Count,
	EGameplayTagReplicationState TagRepState)
{
	ASC->RemoveLooseGameplayTag(GameplayTag, Count, TagRepState);
}
