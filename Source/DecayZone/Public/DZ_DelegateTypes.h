#pragma once

#include "Components/Widget.h"
#include "DZ_DelegateTypes.generated.h"


UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEvent);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetEvent, UWidget*, Widget);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWidgetDragEndedEvent, UWidget*, StartWidget, UWidget*, EndWidget);

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorEvent, AActor*, Actor);
