// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DZ_GameInstance.h"

#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Data/Ability/DZ_SkillDefines.h"
#include "Data/Ability/FDZ_SkillData.h"
#include "DecayZone/DecayZone.h"

void UDZ_GameInstance::Init()
{
	Super::Init();
	if (IsValid(ItemDataTable))
	{
		TArray<FDZ_ItemData*> Rows;
		ItemDataTable->GetAllRows(TEXT("Init ItemMap"), Rows);
		for (const FDZ_ItemData* Data : Rows)
		{
			if (Data->ItemId == DZItem::EmptyId) continue;
			ItemMap.Add(Data->ItemId, Data);
		}
	}
	else
	{
		UE_LOG(LogDZ, Warning, TEXT("Invalid item data table"));
	}
	
	if (IsValid(SkillDataTable))
	{
		for (const TPair<FName, uint8*>& Row : SkillDataTable->GetRowMap())
		{
			const FDZ_SkillData* Data = reinterpret_cast<const FDZ_SkillData*>(Row.Value);
			if (!Data || Data->SkillId == DZSkill::EmptyId) continue;
			SkillMap.Add(Data->SkillId, Data);
		}
	}
	else
	{
		UE_LOG(LogDZ, Warning, TEXT("Invalid skill data table"));
	}
}

const FDZ_ItemData* UDZ_GameInstance::GetItemData(int32 ItemId)
{
	if (ItemId == DZItem::EmptyId) return nullptr;
	if (!ItemMap.Contains(ItemId))
		return nullptr;
	return ItemMap[ItemId];
}

const FDZ_SkillData* UDZ_GameInstance::GetSkillData(int32 SkillId) const
{
	if (SkillId == DZSkill::EmptyId) return nullptr;
	const FDZ_SkillData* const* Found = SkillMap.Find(SkillId);
	return Found ? *Found : nullptr;
}
