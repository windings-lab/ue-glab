#include "GLab_AIController.h"


AGLab_AIController::AGLab_AIController(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	bWantsPlayerState = true;
}
