// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIManager.h"


#include "Library/UIHelper.h"
#include "UI/Controllers/DZ_AbilityBarController.h"
#include "UI/Controllers/DZ_InteractController.h"
#include "UI/Controllers/DZ_InventoryController.h"
#include "UI/Controllers/DZ_LootController.h"
#include "UI/Controllers/DZ_SettlementController.h"
#include "UI/Controllers/DZ_StatusBarsController.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "UI/DZ_UserWidget.h"
#include "Player/DZ_PlayerController.h"

void UUIManager::Initialize(ADZ_PlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController)) return;
	if (PlayerController.Get() == InPlayerController && Controllers.Num() > 0) return;
	
	PlayerController = InPlayerController;
	CreateControllers();
}

void UUIManager::Shutdown()
{
	DestroyControllers();
	
	for (const TWeakObjectPtr<UDZ_UserWidget>& Widget : CurrentActivateWidgets)
	{
		if (Widget.IsValid())
		{
			Widget->RemoveFromParent();
		}
	}
	CurrentActivateWidgets.Reset();
	
	UpdateInputMode();
	PlayerController = nullptr;
}

void UUIManager::TickControllers(float DeltaSeconds)
{
	if (!PlayerController.IsValid()) return;
	
	for (UDZ_UIControllerBase* Controller : Controllers)
	{
		if (IsValid(Controller))
		{
			Controller->Tick(DeltaSeconds);
		}
	}
}

void UUIManager::CreateControllers()
{
	ADZ_PlayerController* PC = PlayerController.Get();
	if (!IsValid(PC)) return;
	
	AbilityBarController = NewObject<UDZ_AbilityBarController>(this);
	AbilityBarController->Initialize(PC);
	Controllers.Add(AbilityBarController);
	
	StatusBarsController = NewObject<UDZ_StatusBarsController>(this);
	StatusBarsController->Initialize(PC);
	Controllers.Add(StatusBarsController);
	
	InteractController = NewObject<UDZ_InteractController>(this);
	InteractController->Initialize(PC);
	Controllers.Add(InteractController);
	
	InventoryController = NewObject<UDZ_InventoryController>(this);
	InventoryController->Initialize(PC);
	Controllers.Add(InventoryController);
	
	LootController = NewObject<UDZ_LootController>(this);
	LootController->Initialize(PC);
	Controllers.Add(LootController);
	
	SettlementController = NewObject<UDZ_SettlementController>(this);
	SettlementController->Initialize(PC);
	Controllers.Add(SettlementController);
}

void UUIManager::DestroyControllers()
{
	for (UDZ_UIControllerBase* Controller : Controllers)
	{
		if (IsValid(Controller))
		{
			Controller->Shutdown();
		}
	}
	Controllers.Reset();
	AbilityBarController = nullptr;
	StatusBarsController = nullptr;
	InteractController = nullptr;
	InventoryController = nullptr;
	LootController = nullptr;
	SettlementController = nullptr;
}

void UUIManager::OpenWidget(UDZ_UserWidget* Widget, bool bResident)
{
	if (!IsValid(Widget)) return;
	
	int32 ZOrder = UIHelper::GetZOrder(Widget);
	
	if (bResident)
	{
		Widget->AddToViewport(ZOrder);
		return;
	}
	
	for (const TWeakObjectPtr<UDZ_UserWidget>& ActiveWidget : CurrentActivateWidgets)
	{
		if (ActiveWidget.Get() == Widget) return;
	}
	
	int32 Pos = CurrentActivateWidgets.Num();
	for (int32 i = 0; i < CurrentActivateWidgets.Num(); i++)
	{
		int32 CurrentZOrder = UIHelper::GetZOrder(CurrentActivateWidgets[i].Get());
		if (CurrentZOrder < ZOrder) continue;
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
	if (!PlayerController.IsValid()) return;
	
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
