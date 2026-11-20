// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Stash/DZ_StashPanel.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "UI/Widgets/Inventory/DZ_EquipmentSlot.h"
#include "UI/Widgets/Item/DZ_ItemSlot.h"

void UDZ_StashPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CollectSlots(Grid_Stash, EItemContainer::Stash, StashSlots);
	CollectSlots(Grid_LoadoutBag, EItemContainer::LoadoutBag, BagSlots);

	EquipSlot_Armor->SetSlotInfo(EItemContainer::LoadoutEquipment, static_cast<int32>(EEquipSlot::Armor));
	EquipSlot_Primary->SetSlotInfo(EItemContainer::LoadoutEquipment, static_cast<int32>(EEquipSlot::Primary));
	EquipSlot_Melee->SetSlotInfo(EItemContainer::LoadoutEquipment, static_cast<int32>(EEquipSlot::Melee));
	EquipSlot_Backpack->SetSlotInfo(EItemContainer::LoadoutEquipment, static_cast<int32>(EEquipSlot::Backpack));

	EquipSlot_Armor->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleItemDrop);
	EquipSlot_Primary->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleItemDrop);
	EquipSlot_Melee->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleItemDrop);
	EquipSlot_Backpack->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleItemDrop);

	Btn_Start->OnClicked.AddDynamic(this, &ThisClass::HandleStartClicked);
	Btn_Shop->OnClicked.AddDynamic(this, &ThisClass::HandleShopClicked);
	Btn_Sell->OnClicked.AddDynamic(this, &ThisClass::HandleSellClicked);

	ClearSellInfo();
}

void UDZ_StashPanel::SetStashItems(const TArray<FDZ_ItemStack>& Items)
{
	for (int32 Index = 0; Index < StashSlots.Num(); Index++)
	{
		if (!IsValid(StashSlots[Index])) continue;

		if (!Items.IsValidIndex(Index))
		{
			StashSlots[Index]->SetItemInfo(DZItem::EmptyId, 0);
			continue;
		}
		StashSlots[Index]->SetItemInfo(Items[Index].ItemId, Items[Index].Count);
	}

	if (SelectedSlot.SlotIndex == INDEX_NONE) return;

	if (!Items.IsValidIndex(SelectedSlot.SlotIndex) || Items[SelectedSlot.SlotIndex].IsEmpty())
	{
		ClearStashSelection();
		return;
	}
	RefreshSlotSelection();
	OnSellTargetChanged.Broadcast(SelectedSlot);
}

void UDZ_StashPanel::SetStashSummary(int32 Used, int32 Capacity)
{
	Text_StashCount->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Used, Capacity)));
}

void UDZ_StashPanel::SetStashTotalValue(int32 Value)
{
	Text_TotalValue->SetText(FText::FromString(FString::Printf(TEXT("$%d"), Value)));
}

void UDZ_StashPanel::SetBagItems(const TArray<FDZ_ItemStack>& Items)
{
	for (int32 Index = 0; Index < BagSlots.Num(); Index++)
	{
		if (!IsValid(BagSlots[Index])) continue;

		if (!Items.IsValidIndex(Index))
		{
			BagSlots[Index]->SetItemInfo(DZItem::EmptyId, 0);
			continue;
		}
		BagSlots[Index]->SetItemInfo(Items[Index].ItemId, Items[Index].Count);
	}
}

void UDZ_StashPanel::SetBagSummary(int32 Used, int32 Capacity)
{
	Text_BagCount->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), Used, Capacity)));
}

void UDZ_StashPanel::SetEquipmentInfo(const TArray<int32>& Equipments)
{
	for (int32 Index = 0; Index < Equipments.Num(); Index++)
	{
		const int32 ItemId = Equipments[Index];
		switch (static_cast<EEquipSlot>(Index))
		{
		case EEquipSlot::Armor:
			EquipSlot_Armor->SetItemInfo(ItemId);
			break;
		case EEquipSlot::Primary:
			EquipSlot_Primary->SetItemInfo(ItemId);
			break;
		case EEquipSlot::Melee:
			EquipSlot_Melee->SetItemInfo(ItemId);
			break;
		case EEquipSlot::Backpack:
			EquipSlot_Backpack->SetItemInfo(ItemId);
			break;
		default:
			break;
		}
	}
}

void UDZ_StashPanel::SetSellInfo(const FText& Info, int32 Price)
{
	Text_SellInfo->SetText(Info);
	Text_SellPrice->SetText(FText::FromString(FString::Printf(TEXT("售价 $%d"), Price)));
	Text_SellPrice->SetVisibility(ESlateVisibility::Visible);
	Btn_Sell->SetIsEnabled(true);
}

void UDZ_StashPanel::ClearSellInfo()
{
	Text_SellInfo->SetText(FText::FromString(TEXT("未选中物品")));
	Text_SellPrice->SetVisibility(ESlateVisibility::Collapsed);
	Btn_Sell->SetIsEnabled(false);
}

void UDZ_StashPanel::CollectSlots(UUniformGridPanel* Grid, EItemContainer Container, TArray<TObjectPtr<UDZ_ItemSlot>>& OutSlots)
{
	OutSlots.Reset();
	if (!IsValid(Grid)) return;

	for (UWidget* Child : Grid->GetAllChildren())
	{
		UDZ_ItemSlot* ItemSlot = Cast<UDZ_ItemSlot>(Child);
		if (!ItemSlot) continue;

		OutSlots.Add(ItemSlot);
		ItemSlot->SetSlotInfo(Container, OutSlots.Num() - 1);
		ItemSlot->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleItemDrop);
		ItemSlot->OnWidgetLeftClicked.AddDynamic(this, &ThisClass::HandleStashSlotClicked);
	}
}

void UDZ_StashPanel::SetStashSelection(const FDZ_ItemSlotHandle& Handle)
{
	SelectedSlot = Handle;
	RefreshSlotSelection();
	OnSellTargetChanged.Broadcast(SelectedSlot);
}

void UDZ_StashPanel::ClearStashSelection()
{
	SelectedSlot = FDZ_ItemSlotHandle();
	RefreshSlotSelection();
	ClearSellInfo();
	OnSellTargetChanged.Broadcast(SelectedSlot);
}

void UDZ_StashPanel::RefreshSlotSelection()
{
	for (UDZ_ItemSlot* ItemSlot : StashSlots)
	{
		if (!IsValid(ItemSlot)) continue;
		ItemSlot->SetSelected(ItemSlot->GetSlotHandle().SlotIndex == SelectedSlot.SlotIndex);
	}
}

void UDZ_StashPanel::HandleItemDrop(UWidget* From, UWidget* To)
{
	OnItemMoveRequested.Broadcast(From, To);
}

void UDZ_StashPanel::HandleStartClicked()
{
	OnStartRequested.Broadcast();
}

void UDZ_StashPanel::HandleShopClicked()
{
	OnShopRequested.Broadcast();
}

void UDZ_StashPanel::HandleStashSlotClicked(UWidget* Widget)
{
	UDZ_ItemSlot* ItemSlot = Cast<UDZ_ItemSlot>(Widget);
	if (!IsValid(ItemSlot)) return;
	if (ItemSlot->GetSlotHandle().Container != EItemContainer::Stash) return;

	if (ItemSlot->IsEmptyState())
	{
		ClearStashSelection();
		return;
	}

	SetStashSelection(ItemSlot->GetSlotHandle());
}

void UDZ_StashPanel::HandleSellClicked()
{
	if (SelectedSlot.SlotIndex == INDEX_NONE) return;

	OnSellRequested.Broadcast(SelectedSlot);
}
