// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_InventoryPanel.h"
#include "Blueprint/WidgetTree.h"
#include "Components/DZ_InventoryComponent.h"
#include "UI/Widgets/DZ_BackpackPanel.h"
#include "UI/Widgets/DZ_ItemDetail.h"
#include "UI/Widgets/DZ_ItemSlot.h"

void UDZ_InventoryPanel::HandleItemDrop(UWidget* From, UWidget* To)
{
	OnItemMoveRequested.Broadcast(From, To);
}

void UDZ_InventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BackpackPanel->OnItemSlotClicked.AddDynamic(this, &ThisClass::HandleItemSlotClicked);
	BackpackPanel->OnItemDrop.AddDynamic(this, &ThisClass::HandleItemDrop);
}

void UDZ_InventoryPanel::SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo)
{
	BackpackPanel->SetBackpackPanel(BackpackInfo);
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
		ItemDetail->SwitchShowType(ItemDetailShowType::Bag);
		CurrentSelectedItem = nullptr;
	}
	else
	{
		ItemDetail->SwitchShowType(ItemDetailShowType::Item);
		ItemDetail->SetItemInfo(ItemSlot->GetItemID());
		ItemSlot->SetSelected(true);
		CurrentSelectedItem = ItemSlot;
	}
}
