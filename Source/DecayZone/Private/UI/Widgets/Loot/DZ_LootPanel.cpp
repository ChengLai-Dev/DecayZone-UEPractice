// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Loot/DZ_LootPanel.h"

#include "UI/Widgets/Inventory/DZ_BackpackPanel.h"
#include "UI/Widgets/Inventory/DZ_EquipmentPanel.h"
#include "UI/Widgets/Loot/DZ_LootListPanel.h"

void UDZ_LootPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	BackpackPanel->OnItemDrop.AddDynamic(this, &ThisClass::HandleItemDrop);
	EquipmentPanel->OnItemDrop.AddDynamic(this, &ThisClass::HandleItemDrop);
	LootListPanel->LootItemMoveRequest.AddDynamic(this, &ThisClass::HandleItemDrop);
	LootListPanel->OnTakeRequested.AddDynamic(this, &ThisClass::HandleTakeRequested);
	LootListPanel->OnCloseRequested.AddDynamic(this, &ThisClass::HandleCloseRequested);
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

void UDZ_LootPanel::SetWeightInfo(float Current, float Max)
{
	BackpackPanel->SetWeightInfo(Current, Max);
}

void UDZ_LootPanel::HandleItemDrop(UWidget* From, UWidget* To)
{
	OnItemMoveRequested.Broadcast(From, To);
}

void UDZ_LootPanel::HandleTakeRequested(UWidget* Row)
{
	OnTakeItemRequested.Broadcast(Row);
}

void UDZ_LootPanel::HandleCloseRequested()
{
	Close();
}
