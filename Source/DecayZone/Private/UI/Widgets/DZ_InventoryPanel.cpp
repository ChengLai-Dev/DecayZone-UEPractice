// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_InventoryPanel.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/UniformGridPanel.h"
#include "UI/Widgets/DZ_ItemDetail.h"
#include "UI/Widgets/DZ_ItemSlot.h"

void UDZ_InventoryPanel::NativeConstruct()
{
	Super::NativeConstruct();
	for (UWidget* Child: Grid_Backpack->GetAllChildren())
	{
		if (Child->GetName().StartsWith("ItemSlot_"))
		{
			if (UDZ_ItemSlot* ItemSlot = Cast<UDZ_ItemSlot>(Child))
			{
				ItemSlots.Add(ItemSlot);
				ItemSlot->SetItemInfo(2);
				ItemSlot->OnWidgetLeftClicked.AddDynamic(this, &ThisClass::OnItemSlotClicked);
			}
		}
	}
}

void UDZ_InventoryPanel::OnItemSlotClicked(UWidget* Widget)
{
	UDZ_ItemSlot* ItemSlot = Cast<UDZ_ItemSlot>(Widget);
	if (CurrentSelectedItem.IsValid())
	{
		CurrentSelectedItem->SetSelected(false);
	}
	if (CurrentSelectedItem == ItemSlot)
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