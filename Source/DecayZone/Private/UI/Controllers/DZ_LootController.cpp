// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_LootController.h"

#include "Blueprint/UserWidget.h"
#include "Components/DZ_ContainerComponent.h"
#include "Components/DZ_InventoryComponent.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Player/DZ_PlayerController.h"
#include "UI/Widgets/Item/DZ_ItemSlotBase.h"
#include "UI/Widgets/Loot/DZ_LootPanel.h"

void UDZ_LootController::Initialize(ADZ_PlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig)) return;

	LootPanelWidget = CreateWidget<UDZ_LootPanel>(PlayerController, UIConfig->LootPanelClass);
	if (!IsValid(LootPanelWidget)) return;

	LootPanelWidget->OnItemMoveRequested.AddDynamic(this, &UDZ_LootController::HandleItemMoveRequested);
	LootPanelWidget->OnTakeItemRequested.AddDynamic(this, &UDZ_LootController::HandleTakeItemRequested);
}

void UDZ_LootController::Shutdown()
{
	if (IsValid(LootPanelWidget))
	{
		LootPanelWidget->OnItemMoveRequested.RemoveAll(this);
		LootPanelWidget->OnTakeItemRequested.RemoveAll(this);
		LootPanelWidget->RemoveFromParent();
	}
	LootPanelWidget = nullptr;
	Super::Shutdown();
}

void UDZ_LootController::Open()
{
	if (IsValid(LootPanelWidget))
	{
		LootPanelWidget->Open();
	}
}

void UDZ_LootController::Refresh(bool bForceRefresh)
{
	if (!IsValid(LootPanelWidget)) return;
	if (!bForceRefresh && !LootPanelWidget->IsInViewport()) return;

	UDZ_InventoryComponent* InventoryComp = PlayerController->GetInventoryComponent();
	if (!InventoryComp) return;

	FDZ_CharacterStatsInfo StatsInfo;
	PlayerController->BuildCharacterStats(StatsInfo);

	LootPanelWidget->SetBackpackPanel(InventoryComp->GetItems());
	LootPanelWidget->SetEquipmentPanel(InventoryComp->GetEquipments());
	LootPanelWidget->SetCharacterStatsInfo(StatsInfo);
	LootPanelWidget->SetWeightInfo(InventoryComp->GetTotalWeight(), InventoryComp->GetMaxWeight());

	if (UDZ_ContainerComponent* Container = PlayerController->GetInteractContainer().Get())
	{
		LootPanelWidget->SetLootListInfo(Container->GetItems());
	}
}

void UDZ_LootController::HandleItemMoveRequested(UWidget* From, UWidget* To)
{
	UDZ_ItemSlotBase* FromWidget = Cast<UDZ_ItemSlotBase>(From);
	UDZ_ItemSlotBase* ToWidget = Cast<UDZ_ItemSlotBase>(To);
	if (!FromWidget || !ToWidget || !PlayerController) return;

	UDZ_InventoryComponent* InventoryComp = PlayerController->GetInventoryComponent();
	if (!InventoryComp) return;

	PlayerController->ServerMoveItem(InventoryComp, PlayerController->GetInteractContainer().Get(),
		FromWidget->GetSlotHandle(), ToWidget->GetSlotHandle());
}

void UDZ_LootController::HandleTakeItemRequested(UWidget* Row)
{
	UDZ_ItemSlotBase* RowWidget = Cast<UDZ_ItemSlotBase>(Row);
	if (!RowWidget || !PlayerController) return;

	UDZ_ContainerComponent* Container = PlayerController->GetInteractContainer().Get();
	if (!Container) return;

	const FDZ_ItemSlotHandle Handle = RowWidget->GetSlotHandle();
	PlayerController->ServerTakeItem(Container->GetOwner(), Handle.SlotIndex, 1);
}
