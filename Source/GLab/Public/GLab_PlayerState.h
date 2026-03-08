#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "GLab_PlayerState.generated.h"

/**
 *
 */
UCLASS()
class GLAB_API AGLab_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AGLab_PlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;
};
