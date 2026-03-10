#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GLab_PlayerController.generated.h"

/**
 *
 */
UCLASS(MinimalAPI)
class AGLab_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AGLab_PlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
