// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DZ_GameInstance.h"

#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "DecayZone/DecayZone.h"

void UDZ_GameInstance::Init()
{
	Super::Init();
	if (!IsValid(ItemDataTable))
	{
		UE_LOG(LogDZ, Warning, TEXT("Invalid item data table"));
		return;
	}
	TArray<FDZ_ItemData*> Rows;
	ItemDataTable->GetAllRows(TEXT("Init ItemMap"), Rows);
	for (const FDZ_ItemData* Data : Rows)
	{
		if (Data->ItemId == DZItem::EmptyId) continue;
		ItemMap.Add(Data->ItemId, Data);
	}
}

const FDZ_ItemData* UDZ_GameInstance::GetItemData(int32 ItemId)
{
	if (ItemId == DZItem::EmptyId) return nullptr;
	if (!ItemMap.Contains(ItemId))
		return nullptr;
	return ItemMap[ItemId];
}
