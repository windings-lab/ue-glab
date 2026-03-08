#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "GLab_AIController.generated.h"

UCLASS()
class GLAB_API AGLab_AIController : public AAIController
{
	GENERATED_BODY()

public:
	AGLab_AIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
