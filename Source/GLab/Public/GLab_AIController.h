#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "GLab_AIController.generated.h"

UCLASS(MinimalAPI)
class AGLab_AIController : public AAIController
{
	GENERATED_BODY()

public:
	AGLab_AIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
