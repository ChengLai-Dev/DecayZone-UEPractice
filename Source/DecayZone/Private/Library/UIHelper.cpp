#include "Library/UIHelper.h"

#include "Blueprint/UserWidget.h"
#include "Components/PanelWidget.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Game/DZ_GameInstance.h"

UWidget* UIHelper::GetChildByName(const UWidget* Widget, const FName& Name)
{
	if (!Widget || Name.IsNone()) return nullptr;
	
	const UPanelWidget* PanelWidget = Cast<UPanelWidget>(Widget);
	if (!PanelWidget) return nullptr;
	
	for (UWidget* Child : PanelWidget->GetAllChildren())
	{
		if (Child->GetName() == Name)
		{
			return Child;
		}
	}
	return nullptr;
}

UWidget* UIHelper::SeekWidget(const UWidget* Widget, const FString& Path)
{
	if (!Widget) return nullptr;
	TArray<FString> PathNodes;
	Path.ParseIntoArray(PathNodes, TEXT("."));
	
	UWidget* CurrentWidget = const_cast<UWidget*>(Widget);

	for (int32 i = 0; i < PathNodes.Num(); ++i)
	{
		CurrentWidget = GetChildByName(CurrentWidget, FName(*PathNodes[i]));
		if (!CurrentWidget) return nullptr;
	} 

	return CurrentWidget;
}

int32 UIHelper::GetZOrder(const UUserWidget* Widget)
{
	if (!Widget) return 0;
	
	UDZ_GameInstance* GameInstance = Cast<UDZ_GameInstance>(Widget->GetGameInstance());
	int32* ZOrder = GameInstance->UIConfig->ZOrderMap.Find(Widget->GetClass());
	if (!ZOrder) return 0;
	return *ZOrder;
}
