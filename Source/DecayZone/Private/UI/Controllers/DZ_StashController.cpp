// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_StashController.h"

#include "Blueprint/UserWidget.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Game/DZ_GameInstance.h"
#include "Player/DZ_LobbyPlayerController.h"
#include "UI/Controllers/DZ_ShopController.h"
#include "UI/DZ_LobbyUIManager.h"
#include "UI/Widgets/Item/DZ_ItemSlotBase.h"
#include "UI/Widgets/Stash/DZ_StashPanel.h"

void UDZ_StashController::Initialize(APlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig) || !IsValid(GameInstance)) return;
	
	LobbyPlayerController = Cast<ADZ_LobbyPlayerController>(PlayerController);
	if (!IsValid(LobbyPlayerController)) return;

	GameInstance->OnStashChanged.AddDynamic(this, &UDZ_StashController::HandleStashChanged);

	StashPanelWidget = CreateWidget<UDZ_StashPanel>(LobbyPlayerController, UIConfig->StashWidgetClass);
	if (!IsValid(StashPanelWidget)) return;

	StashPanelWidget->OnItemMoveRequested.AddDynamic(this, &UDZ_StashController::HandleItemMoveRequested);
	StashPanelWidget->OnStartRequested.AddDynamic(this, &UDZ_StashController::HandleStartRequested);
	StashPanelWidget->OnShopRequested.AddDynamic(this, &UDZ_StashController::HandleShopRequested);
	StashPanelWidget->OnSellTargetChanged.AddDynamic(this, &UDZ_StashController::HandleSellTargetChanged);
	StashPanelWidget->OnSellRequested.AddDynamic(this, &UDZ_StashController::HandleSellRequested);
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
		StashPanelWidget->OnShopRequested.RemoveAll(this);
		StashPanelWidget->OnSellTargetChanged.RemoveAll(this);
		StashPanelWidget->OnSellRequested.RemoveAll(this);
		StashPanelWidget->RemoveFromParent();
	}
	StashPanelWidget = nullptr;
	LobbyPlayerController = nullptr;
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
	StashPanelWidget->SetBagItems(GameInstance->GetLoadoutBagItems());
	StashPanelWidget->SetBagSummary(GameInstance->GetStashBagUsedSlots(), GameInstance->GetStashBagCapacity());
	StashPanelWidget->SetEquipmentInfo(GameInstance->GetLoadoutEquipments());
}

void UDZ_StashController::HandleSellTargetChanged(FDZ_ItemSlotHandle Handle)
{
	if (!IsValid(StashPanelWidget) || !IsValid(GameInstance)) return;

	const TArray<FDZ_ItemStack>& StashItems = GameInstance->GetStashItems();
	if (Handle.SlotIndex == INDEX_NONE || !StashItems.IsValidIndex(Handle.SlotIndex))
	{
		StashPanelWidget->ClearSellInfo();
		return;
	}

	const FDZ_ItemStack& Item = StashItems[Handle.SlotIndex];
	if (Item.IsEmpty())
	{
		StashPanelWidget->ClearSellInfo();
		return;
	}

	const FDZ_ItemData* Data = GameInstance->GetItemData(Item.ItemId);
	if (!Data)
	{
		StashPanelWidget->ClearSellInfo();
		return;
	}

	const int32 Price = GameInstance->GetSellPrice(Item.ItemId);
	const FText Info = FText::FromString(FString::Printf(TEXT("%s ×%d"), *Data->ItemName.ToString(), Item.Count));
	StashPanelWidget->SetSellInfo(Info, Price);
}

void UDZ_StashController::HandleItemMoveRequested(UWidget* From, UWidget* To)
{
	UDZ_ItemSlotBase* FromWidget = Cast<UDZ_ItemSlotBase>(From);
	UDZ_ItemSlotBase* ToWidget = Cast<UDZ_ItemSlotBase>(To);
	if (!FromWidget || !ToWidget || !IsValid(GameInstance) || !IsValid(LobbyPlayerController)) return;
	
	FText FailReason;
	if (GameInstance->MoveStashItem(FromWidget->GetSlotHandle(), ToWidget->GetSlotHandle(), FailReason)) return;
	if (FailReason.IsEmpty()) return;
	
	LobbyPlayerController->ShowTips(FailReason, 2.f);
}

void UDZ_StashController::HandleStartRequested()
{
	OnDepartRequested.Broadcast();
	
	if (IsValid(LobbyPlayerController))
	{
		LobbyPlayerController->StartRaid();
	}
}

void UDZ_StashController::HandleShopRequested()
{
	UDZ_LobbyUIManager* LobbyUIManager = Cast<UDZ_LobbyUIManager>(GetOwningManager());
	if (!IsValid(LobbyUIManager) || !IsValid(LobbyUIManager->ShopController)) return;

	LobbyUIManager->ShopController->Toggle();
}

void UDZ_StashController::HandleSellRequested(FDZ_ItemSlotHandle Handle)
{
	if (!IsValid(GameInstance)) return;
	if (Handle.SlotIndex == INDEX_NONE) return;

	FText FailReason;
	if (GameInstance->TrySellStashSlot(Handle, 1, FailReason)) return;
	if (!FailReason.IsEmpty() && IsValid(LobbyPlayerController))
	{
		LobbyPlayerController->ShowTips(FailReason, 2.f);
	}
}

void UDZ_StashController::HandleStashChanged()
{
	Refresh();
}
