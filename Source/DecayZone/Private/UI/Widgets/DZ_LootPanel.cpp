// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_LootPanel.h"

#include "UI/Widgets/DZ_BackpackPanel.h"
#include "UI/Widgets/DZ_EquipmentPanel.h"
#include "UI/Widgets/DZ_LootListPanel.h"

void UDZ_LootPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BackpackPanel->OnItemDrop.AddDynamic(this, &ThisClass::HandleItemDrop);
	LootListPanel->LootItemMoveRequest.AddDynamic(this, &ThisClass::HandleItemDrop);
}

void UDZ_LootPanel::SetLootListInfo(const TArray<FDZ_ItemStack>& LootList)
{
	LootListPanel->SetLootListInfo(LootList);
}

void UDZ_LootPanel::SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo)
{
	BackpackPanel->SetBackpackPanel(BackpackInfo);
}

void UDZ_LootPanel::SetEquipmentPanel(const TArray<int32>& EquipmentInfo)
{
	EquipmentPanel->SetEquipmentInfo(EquipmentInfo);
}

void UDZ_LootPanel::SetCharacterStatsInfo(const FDZ_CharacterStatsInfo& StatsInfo)
{
	EquipmentPanel->SetCharacterStatsInfo(StatsInfo);
}

void UDZ_LootPanel::HandleItemDrop(UWidget* From, UWidget* To)
{
	OnItemMoveRequested.Broadcast(From, To);
}
