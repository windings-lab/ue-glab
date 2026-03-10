#include "Components/GLab_AbilitySetComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"


UGLab_AbilitySetComponent::UGLab_AbilitySetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGLab_AbilitySetComponent::OnRegister()
{
	Super::OnRegister();

	ASI = GetOwner();
	if (!ASI)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("%s: Owner is not implementing IAbilitySystemInterface. Destroying component."),
			*GetName()
		);
		DestroyComponent();
	}
}

void UGLab_AbilitySetComponent::BeginPlay()
{
	Super::BeginPlay();

	GrantAbilities();
}

void UGLab_AbilitySetComponent::GrantAbilities()
{
	auto ASC = ASI->GetAbilitySystemComponent();

	for (auto& AbilitySpecDef : AbilitiesToGrant)
	{
		auto AbilitySpec = FGameplayAbilitySpec(AbilitySpecDef, AbilitySpecDef.LevelScalableFloat.Value);
		ASC->GiveAbility(AbilitySpec);
	}
}
