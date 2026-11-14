// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Inventory/DZ_EquipmentPanel.h"

#include "UI/Widgets/Inventory/DZ_CharacterStats.h"
#include "UI/Widgets/Inventory/DZ_EquipmentSlot.h"

void UDZ_EquipmentPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	EquipSlotMap = {
		{EEquipSlot::Armor, EquipSlot_Armor},
		{EEquipSlot::Primary, EquipSlot_Primary},
		{EEquipSlot::Backpack, EquipSlot_Backpack},
		{EEquipSlot::Melee, EquipSlot_Melee},
	};
	EquipSlot_Armor->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
	EquipSlot_Primary->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
	EquipSlot_Backpack->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
	EquipSlot_Melee->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleEquipSlotDrop);
	
	EquipSlot_Armor->SetSlotInfo(EItemContainer::Equipment, static_cast<int32>(EEquipSlot::Armor));
	EquipSlot_Primary->SetSlotInfo(EItemContainer::Equipment, static_cast<int32>(EEquipSlot::Primary));
	EquipSlot_Backpack->SetSlotInfo(EItemContainer::Equipment, static_cast<int32>(EEquipSlot::Backpack));
	EquipSlot_Melee->SetSlotInfo(EItemContainer::Equipment, static_cast<int32>(EEquipSlot::Melee));
}

void UDZ_EquipmentPanel::SetItemInfo(int32 ItemId, EEquipSlot EquipSlot)
{
	if (EquipSlot == EEquipSlot::None) return;
	TWeakObjectPtr<UDZ_EquipmentSlot> EquipSlotWidget = EquipSlotMap[EquipSlot];
	EquipSlotWidget->SetItemInfo(ItemId);
}

void UDZ_EquipmentPanel::SetEquipmentInfo(const TArray<int32>& Equipments)
{
	for (int32 i = 0; i < Equipments.Num(); i++)
	{
		EEquipSlot EquipSlot = static_cast<EEquipSlot>(i);
		int32 ItemId = Equipments[i];
		if (!EquipSlotMap.Find(EquipSlot)) continue;
		SetItemInfo(ItemId, EquipSlot);
	}
}

void UDZ_EquipmentPanel::SetCharacterStatsInfo(const FDZ_CharacterStatsInfo& StatsInfo)
{
	CharacterStats->SetHealth(StatsInfo.Health, StatsInfo.MaxHealth);
	CharacterStats->SetArmor(StatsInfo.Armor);
	CharacterStats->SetEnergy(StatsInfo.Energy, StatsInfo.MaxEnergy);
	CharacterStats->SetWeight(StatsInfo.Weight, StatsInfo.MaxWeight);
}

void UDZ_EquipmentPanel::HandleEquipSlotDrop(UWidget* From, UWidget* To)
{
	OnItemDrop.Broadcast(From, To);
}
