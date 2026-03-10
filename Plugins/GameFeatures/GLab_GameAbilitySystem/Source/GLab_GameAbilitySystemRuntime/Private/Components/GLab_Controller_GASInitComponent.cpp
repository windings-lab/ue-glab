#include "Components/GLab_Controller_GASInitComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UGLab_Controller_GASInitComponent::UGLab_Controller_GASInitComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

#if WITH_EDITOR
EDataValidationResult UGLab_Controller_GASInitComponent::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	auto ControllerOwner = GetTypedOuter<UBlueprintGeneratedClass>();
	if (!ControllerOwner->IsChildOf(AController::StaticClass()))
	{
		Context.AddError(NSLOCTEXT("GLAB_API", "InvalidOwner", "UGLab_Controller_GASInitComponent must be attached to a Controller."));
		Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
	}

	return Result;
}
#endif // WITH_EDITOR

void UGLab_Controller_GASInitComponent::OnRegister()
{
	Super::OnRegister();

	auto ControllerOwner = GetOwner<AController>();
	if (!ControllerOwner) return;

	if (!ControllerOwner->GetOnNewPawnNotifier().IsBoundToObject(this))
	{
		ControllerOwner->GetOnNewPawnNotifier().AddUObject(this, &ThisClass::OnPossess);
	}
}

void UGLab_Controller_GASInitComponent::OnPossess(APawn* NewPawn)
{
	auto ASIPawn = Cast<IAbilitySystemInterface>(NewPawn);
	if (!ASIPawn) return;

	ASIPawn->GetAbilitySystemComponent()->InitAbilityActorInfo(GetOwner(), NewPawn);
}
