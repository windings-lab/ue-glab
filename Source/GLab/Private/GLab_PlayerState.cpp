#include "GLab_PlayerState.h"

#include "AbilitySystemComponent.h"

AGLab_PlayerState::AGLab_PlayerState(const FObjectInitializer& ObjectInitializer)
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(L"Ability System Component");
}
