// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_EquipmentPanel.h"

#include "UI/Widgets/DZ_EquipmentSlot.h"

void UDZ_EquipmentPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	EquipSlot_Armor->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
	EquipSlot_Primary->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
	EquipSlot_Backpack->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
	EquipSlot_Melee->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
}

void UDZ_EquipmentPanel::SetItemInfo(int32 ItemId, EEquipSlot EquipSlot)
{
	if (EquipSlot == EEquipSlot::None) return;
	TWeakObjectPtr<UDZ_EquipmentSlot> EquipSlotWidget = EquipSlotMap[EquipSlot];
	EquipSlotWidget->SetItemInfo(ItemId);
}

void UDZ_EquipmentPanel::HandleEquipSlotDrop(UWidget* From, UWidget* To)
{
	OnItemDrop.Broadcast(From, To);
}
