// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GLab_Character.generated.h"

UCLASS(MinimalAPI)
class AGLab_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGLab_Character();

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
};
