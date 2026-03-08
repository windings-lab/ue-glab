// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GLab_Character.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

// Sets default values
AGLab_Character::AGLab_Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGLab_Character::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGLab_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGLab_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AGLab_Character::GetAbilitySystemComponent() const
{
	const APlayerState* PS = GetPlayerState();
	if (!PS)
		return nullptr;

	const auto ASC = PS->FindComponentByClass<UAbilitySystemComponent>();
	if (!ASC)
		return nullptr;

	return ASC;
}

