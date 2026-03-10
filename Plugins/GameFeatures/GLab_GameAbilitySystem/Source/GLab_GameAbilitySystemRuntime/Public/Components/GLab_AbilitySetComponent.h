#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Components/ActorComponent.h"
#include "GLab_AbilitySetComponent.generated.h"


UCLASS(meta=(BlueprintSpawnableComponent))
class GLAB_GAMEABILITYSYSTEMRUNTIME_API UGLab_AbilitySetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGLab_AbilitySetComponent();

	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayAbilitySpecDef> AbilitiesToGrant;

private:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

	void GrantAbilities();

	TScriptInterface<class IAbilitySystemInterface> ASI;
};
