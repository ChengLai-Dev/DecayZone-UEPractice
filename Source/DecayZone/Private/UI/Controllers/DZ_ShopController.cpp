// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_ShopController.h"

#include "Data/Item/FDZ_ItemData.h"
#include "Data/Shop/DZ_ShopItemObject.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Game/DZ_GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Player/DZ_PlayerController.h"
#include "UI/Widgets/Shop/DZ_ShopPanel.h"

namespace
{
	FText GetItemTypeText(const FDZ_ItemData* Data)
	{
		if (!Data) return FText::GetEmpty();

		switch (Data->EquipSlot)
		{
		case EEquipSlot::Armor:
			return FText::FromString(TEXT("护甲"));
		case EEquipSlot::Primary:
			return FText::FromString(TEXT("主武器"));
		case EEquipSlot::Melee:
			return FText::FromString(TEXT("近战武器"));
		case EEquipSlot::Backpack:
			return FText::FromString(TEXT("背包"));
		default:
			break;
		}

		return Data->ItemType == EItemType::Equipment
			? FText::FromString(TEXT("装备"))
			: FText::FromString(TEXT("物资"));
	}
}

void UDZ_ShopController::Initialize(APlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig) || !IsValid(GameInstance) || !IsValid(PlayerController)) return;

	ShopPanelWidget = CreateWidget<UDZ_ShopPanel>(PlayerController, UIConfig->ShopWidgetClass);
	if (!IsValid(ShopPanelWidget)) return;

	ShopPanelWidget->OnBuyRequested.AddDynamic(this, &UDZ_ShopController::HandleBuyRequested);
	GameInstance->OnMoneyChanged.AddDynamic(this, &UDZ_ShopController::HandleMoneyChanged);
	GameInstance->OnStashChanged.AddDynamic(this, &UDZ_ShopController::HandleStashChanged);

	BuildEntries();
}

void UDZ_ShopController::Shutdown()
{
	if (IsValid(ShopPanelWidget))
	{
		ShopPanelWidget->OnBuyRequested.RemoveAll(this);
		ShopPanelWidget->RemoveFromParent();
	}
	ShopPanelWidget = nullptr;

	if (IsValid(GameInstance))
	{
		GameInstance->OnMoneyChanged.RemoveAll(this);
		GameInstance->OnStashChanged.RemoveAll(this);
	}

	Entries.Reset();
	Super::Shutdown();
}

void UDZ_ShopController::Open()
{
	if (!IsValid(ShopPanelWidget)) return;

	Refresh();
	ShopPanelWidget->Open();
}

void UDZ_ShopController::Close()
{
	if (!IsValid(ShopPanelWidget)) return;

	ShopPanelWidget->Close();
}

bool UDZ_ShopController::Toggle()
{
	if (!IsValid(ShopPanelWidget)) return false;

	if (ShopPanelWidget->IsInViewport())
	{
		Close();
		return false;
	}

	Open();
	return true;
}

void UDZ_ShopController::Refresh()
{
	if (!IsValid(ShopPanelWidget) || !IsValid(GameInstance)) return;
	if (Entries.Num() == 0) return;

	for (const TObjectPtr<UDZ_ShopItemObject>& Entry : Entries)
	{
		if (!IsValid(Entry)) continue;

		FText FailReason;
		Entry->bCanBuy = GameInstance->CanBuyShopItem(Entry->ItemId, 1, FailReason);
	}

	ShopPanelWidget->SetMoney(GameInstance->GetMoney());
	ShopPanelWidget->RefreshEntries();
}

void UDZ_ShopController::BuildEntries()
{
	Entries.Reset();
	for (const FDZ_ShopItemData& ShopItem : GameInstance->GetShopItems())
	{
		if (!ShopItem.bEnabled) continue;

		const FDZ_ItemData* Data = GameInstance->GetItemData(ShopItem.ItemId);
		if (!Data) continue;

		FText FailReason;
		const bool bCanBuy = GameInstance->CanBuyShopItem(ShopItem.ItemId, 1, FailReason);

		UDZ_ShopItemObject* Entry = NewObject<UDZ_ShopItemObject>(ShopPanelWidget);
		Entry->ItemId = ShopItem.ItemId;
		Entry->Name = Data->ItemName;
		Entry->TypeText = GetItemTypeText(Data);
		Entry->Icon = Data->Icon;
		Entry->Price = ShopItem.Price;
		Entry->bCanBuy = bCanBuy;

		Entries.Add(Entry);
	}

	ShopPanelWidget->SetMoney(GameInstance->GetMoney());
	ShopPanelWidget->SetShopItems(Entries);
}

void UDZ_ShopController::HandleBuyRequested(int32 ItemId)
{
	if (!IsValid(GameInstance)) return;

	FText FailReason;
	if (GameInstance->TryBuyShopItem(ItemId, 1, FailReason)) return;
	if (!FailReason.IsEmpty())
	{
		ShowTips(FailReason);
	}
}

void UDZ_ShopController::HandleMoneyChanged()
{
	Refresh();
}

void UDZ_ShopController::HandleStashChanged()
{
	Refresh();
}

void UDZ_ShopController::ShowTips(const FText& TipText)
{
	ADZ_PlayerController* DZPlayerController = Cast<ADZ_PlayerController>(PlayerController);
	if (!IsValid(DZPlayerController)) return;

	DZPlayerController->ShowTips(TipText, 2.f);
}
