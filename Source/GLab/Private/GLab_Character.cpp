#include "GLab_Character.h"

#include "AbilitySystemComponent.h"

AGLab_Character::AGLab_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
}

void AGLab_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UAbilitySystemComponent* AGLab_Character::GetAbilitySystemComponent() const
{
	return ASC;
}

