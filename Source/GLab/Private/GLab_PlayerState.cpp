#include "GLab_PlayerState.h"

#include "AbilitySystemComponent.h"

AGLab_PlayerState::AGLab_PlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(L"Ability System Component");
}

UAbilitySystemComponent* AGLab_PlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
