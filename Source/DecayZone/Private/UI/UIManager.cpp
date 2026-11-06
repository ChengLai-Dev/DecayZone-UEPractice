// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIManager.h"

#include "Library/UIHelper.h"
#include "UI/DZ_UserWidget.h"

void UUIManager::OpenWidget(UDZ_UserWidget* Widget, bool bEnableKeyClose)
{
	int32 ZOrder = UIHelper::GetZOrder(Widget);
	
	if (!bEnableKeyClose)
	{
		Widget->AddToViewport(ZOrder);
		return;
	}
	
	int32 Pos = CurrentActivateWidgets.Num();
	for (int32 i = 0; i < CurrentActivateWidgets.Num(); i++)
	{
		int32 CurrentZOrder = UIHelper::GetZOrder(CurrentActivateWidgets[i].Get());
		if (CurrentZOrder < ZOrder) continue;
		if (Widget == CurrentActivateWidgets[i].Get()) return;
		if (CurrentZOrder == ZOrder) continue;
		
		Pos = i;
		break;
	}
	CurrentActivateWidgets.Insert(Widget, Pos);
	Widget->AddToViewport(ZOrder);
	UpdateInputMode();
}

void UUIManager::CloseWidget(UDZ_UserWidget* Widget)
{
	for (TWeakObjectPtr<UDZ_UserWidget>& CurrentWidget : CurrentActivateWidgets)
	{
		if (CurrentWidget == Widget)
		{
			CurrentActivateWidgets.Remove(Widget);
			break;
		}
	}
	Widget->RemoveFromParent();
	UpdateInputMode();
}

void UUIManager::UpdateInputMode()
{
	APlayerController* PlayerController = GetLocalPlayer()->GetPlayerController(GetWorld());
	if (CurrentActivateWidgets.IsEmpty())
	{
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
	}
	else
	{
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->SetShowMouseCursor(true);
	}
}

void UUIManager::CloseTopWidget()
{
	if (CurrentActivateWidgets.IsEmpty()) return;
	TWeakObjectPtr<UDZ_UserWidget> TopWidget = CurrentActivateWidgets.Pop();
	if (TopWidget.IsValid())
	{
		TopWidget->RemoveFromParent();
	}
	UpdateInputMode();
}

