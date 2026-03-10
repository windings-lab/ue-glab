#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GLab_Controller_GASInitComponent.generated.h"


UCLASS(meta=(BlueprintSpawnableComponent))
class GLAB_GAMEABILITYSYSTEMRUNTIME_API UGLab_Controller_GASInitComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGLab_Controller_GASInitComponent();

protected:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR
	virtual void OnRegister() override;

private:
	UFUNCTION()
	void OnPossess(APawn* NewPawn);
};
