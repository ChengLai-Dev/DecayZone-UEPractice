// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Inventory/DZ_InventoryPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/DZ_InventoryComponent.h"
#include "UI/Widgets/Inventory/DZ_BackpackPanel.h"
#include "UI/Widgets/Inventory/DZ_EquipmentPanel.h"
#include "UI/Widgets/Item/DZ_ItemDetail.h"
#include "UI/Widgets/Item/DZ_ItemSlot.h"

void UDZ_InventoryPanel::HandleItemDrop(UWidget* From, UWidget* To)
{
	OnItemMoveRequested.Broadcast(From, To);
}

void UDZ_InventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BackpackPanel->OnItemSlotClicked.AddDynamic(this, &ThisClass::HandleItemSlotClicked);
	BackpackPanel->OnItemDrop.AddDynamic(this, &ThisClass::HandleItemDrop);
	EquipmentPanel->OnItemDrop.AddDynamic(this, &ThisClass::HandleItemDrop);
}

void UDZ_InventoryPanel::SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo)
{
	BackpackPanel->SetBackpackPanel(BackpackInfo);
	RefreshItemDetail();
}

void UDZ_InventoryPanel::SetEquipmentPanel(const TArray<int32>& EquipmentInfo)
{
	EquipmentPanel->SetEquipmentInfo(EquipmentInfo);
}

void UDZ_InventoryPanel::SetCharacterStatsInfo(const FDZ_CharacterStatsInfo& StatsInfo)
{
	EquipmentPanel->SetCharacterStatsInfo(StatsInfo);
}

void UDZ_InventoryPanel::SetWeightInfo(float Current, float Max)
{
	BackpackPanel->SetWeightInfo(Current, Max);
}

void UDZ_InventoryPanel::HandleItemSlotClicked(UWidget* Widget)
{
	UDZ_ItemSlot* ItemSlot = Cast<UDZ_ItemSlot>(Widget);
	if (CurrentSelectedItem.IsValid())
	{
		CurrentSelectedItem->SetSelected(false);
	}
	if (CurrentSelectedItem == ItemSlot || ItemSlot->IsEmptyState())
	{
		CurrentSelectedItem = nullptr;
	}
	else
	{
		CurrentSelectedItem = ItemSlot;
		ItemSlot->SetSelected(true);
	}
	RefreshItemDetail();
}

void UDZ_InventoryPanel::RefreshItemDetail()
{
	if (!CurrentSelectedItem.IsValid())
	{
		ItemDetail->SwitchShowType(ItemDetailShowType::Bag);
		return;
	}
	ItemDetail->SwitchShowType(ItemDetailShowType::Item);
	ItemDetail->SetItemInfo(CurrentSelectedItem->GetItemID());
}