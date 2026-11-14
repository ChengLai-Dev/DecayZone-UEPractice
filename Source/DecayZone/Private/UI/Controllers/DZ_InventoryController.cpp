// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_InventoryController.h"

#include "Blueprint/UserWidget.h"
#include "Components/DZ_ContainerComponent.h"
#include "Components/DZ_InventoryComponent.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Player/DZ_PlayerController.h"
#include "UI/Widgets/Inventory/DZ_InventoryPanel.h"
#include "UI/Widgets/Item/DZ_ItemSlotBase.h"

void UDZ_InventoryController::Initialize(ADZ_PlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig)) return;

	InventoryPanelWidget = CreateWidget<UDZ_InventoryPanel>(PlayerController, UIConfig->InventoryWidgetClass);
	if (!IsValid(InventoryPanelWidget)) return;

	InventoryPanelWidget->OnItemMoveRequested.AddDynamic(this, &UDZ_InventoryController::HandleItemMoveRequested);
}

void UDZ_InventoryController::Shutdown()
{
	if (IsValid(InventoryPanelWidget))
	{
		InventoryPanelWidget->OnItemMoveRequested.RemoveAll(this);
		InventoryPanelWidget->RemoveFromParent();
	}
	InventoryPanelWidget = nullptr;
	Super::Shutdown();
}

bool UDZ_InventoryController::Toggle()
{
	if (!IsValid(InventoryPanelWidget)) return false;
	if (!PlayerController) return false;

	if (InventoryPanelWidget->IsInViewport())
	{
		InventoryPanelWidget->Close();
		return false;
	}

	InventoryPanelWidget->Open();
	return true;
}

void UDZ_InventoryController::Refresh(bool bForceRefresh)
{
	if (!IsValid(InventoryPanelWidget)) return;
	if (!bForceRefresh && !InventoryPanelWidget->IsInViewport()) return;

	UDZ_InventoryComponent* InventoryComp = PlayerController->GetInventoryComponent();
	if (!InventoryComp) return;

	FDZ_CharacterStatsInfo StatsInfo;
	PlayerController->BuildCharacterStats(StatsInfo);

	InventoryPanelWidget->SetBackpackPanel(InventoryComp->GetItems());
	InventoryPanelWidget->SetEquipmentPanel(InventoryComp->GetEquipments());
	InventoryPanelWidget->SetCharacterStatsInfo(StatsInfo);
	InventoryPanelWidget->SetWeightInfo(InventoryComp->GetTotalWeight(), InventoryComp->GetMaxWeight());
}

void UDZ_InventoryController::HandleItemMoveRequested(UWidget* From, UWidget* To)
{
	UDZ_ItemSlotBase* FromWidget = Cast<UDZ_ItemSlotBase>(From);
	UDZ_ItemSlotBase* ToWidget = Cast<UDZ_ItemSlotBase>(To);
	if (!FromWidget || !ToWidget || !PlayerController) return;

	UDZ_InventoryComponent* InventoryComp = PlayerController->GetInventoryComponent();
	if (!InventoryComp) return;

	PlayerController->ServerMoveItem(InventoryComp, PlayerController->GetInteractContainer().Get(),
		FromWidget->GetSlotHandle(), ToWidget->GetSlotHandle());
}
