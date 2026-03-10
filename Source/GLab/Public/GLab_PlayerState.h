#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "GLab_PlayerState.generated.h"

/**
 *
 */
UCLASS(MinimalAPI)
class AGLab_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGLab_PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
};
