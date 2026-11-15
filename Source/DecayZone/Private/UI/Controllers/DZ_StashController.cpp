// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_StashController.h"

#include "Blueprint/UserWidget.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Game/DZ_GameInstance.h"
#include "Player/DZ_PlayerController.h"
#include "UI/Widgets/Item/DZ_ItemSlotBase.h"
#include "UI/Widgets/Stash/DZ_StashPanel.h"

void UDZ_StashController::Initialize(ADZ_PlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig) || !IsValid(GameInstance)) return;

	GameInstance->OnStashChanged.AddDynamic(this, &UDZ_StashController::HandleStashChanged);

	StashPanelWidget = CreateWidget<UDZ_StashPanel>(PlayerController, UIConfig->StashWidgetClass);
	if (!IsValid(StashPanelWidget)) return;

	StashPanelWidget->OnItemMoveRequested.AddDynamic(this, &UDZ_StashController::HandleItemMoveRequested);
	StashPanelWidget->OnStartRequested.AddDynamic(this, &UDZ_StashController::HandleStartRequested);
}

void UDZ_StashController::Shutdown()
{
	if (IsValid(GameInstance))
	{
		GameInstance->OnStashChanged.RemoveAll(this);
	}

	if (IsValid(StashPanelWidget))
	{
		StashPanelWidget->OnItemMoveRequested.RemoveAll(this);
		StashPanelWidget->OnStartRequested.RemoveAll(this);
		StashPanelWidget->RemoveFromParent();
	}
	StashPanelWidget = nullptr;
	Super::Shutdown();
}

void UDZ_StashController::Open()
{
	if (!IsValid(StashPanelWidget)) return;

	Refresh();
	StashPanelWidget->Open();
}

void UDZ_StashController::Close()
{
	if (!IsValid(StashPanelWidget)) return;

	StashPanelWidget->Close();
}

bool UDZ_StashController::Toggle()
{
	if (!IsValid(StashPanelWidget)) return false;

	if (StashPanelWidget->IsInViewport())
	{
		Close();
		return false;
	}

	Open();
	return true;
}

void UDZ_StashController::Refresh()
{
	if (!IsValid(StashPanelWidget) || !IsValid(GameInstance)) return;

	StashPanelWidget->SetStashItems(GameInstance->GetStashItems());
	StashPanelWidget->SetStashSummary(GameInstance->GetStashUsedSlots(), GameInstance->GetStashCapacity());
	StashPanelWidget->SetStashTotalValue(GameInstance->GetStashTotalValue());
	StashPanelWidget->SetBagItems(GameInstance->GetStashBagItems());
	StashPanelWidget->SetBagSummary(GameInstance->GetStashBagUsedSlots(), GameInstance->GetStashBagCapacity());
	StashPanelWidget->SetEquipmentInfo(GameInstance->GetStashEquipments());
}

void UDZ_StashController::HandleItemMoveRequested(UWidget* From, UWidget* To)
{
	UDZ_ItemSlotBase* FromWidget = Cast<UDZ_ItemSlotBase>(From);
	UDZ_ItemSlotBase* ToWidget = Cast<UDZ_ItemSlotBase>(To);
	if (!FromWidget || !ToWidget || !IsValid(GameInstance) || !PlayerController) return;
	
	FText FailReason;
	if (GameInstance->MoveStashItem(FromWidget->GetSlotHandle(), ToWidget->GetSlotHandle(), FailReason)) return;
	if (FailReason.IsEmpty()) return;
	
	PlayerController->ShowTips(FailReason, 2.f);
}

void UDZ_StashController::HandleStartRequested()
{
	OnDepartRequested.Broadcast();
}

void UDZ_StashController::HandleStashChanged()
{
	Refresh();
}
