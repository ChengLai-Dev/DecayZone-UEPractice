// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DZ_GameInstance.h"

#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "Data/Ability/DZ_SkillDefines.h"
#include "Data/Ability/FDZ_SkillData.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Data/UI/FDZ_AbilitySlotLayout.h"
#include "Library/ItemHelper.h"
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
	
	InitSlotSkills();
	
	StashItems.Reset();
	for (int32 Index = 0; Index < StashCapacity; Index++)
	{
		StashItems.Add(FDZ_ItemStack());
	}
	
	StashBagItems.Reset();
	for (int32 Index = 0; Index < StashBagCapacity; Index++)
	{
		StashBagItems.Add(FDZ_ItemStack());
	}
	
	StashEquipments.Reset();
	for (int32 Index = 0; Index < static_cast<int32>(EEquipSlot::None); Index++)
	{
		StashEquipments.Add(DZItem::EmptyId);
	}
}

void UDZ_GameInstance::InitSlotSkills()
{
	if (!IsValid(UIConfig))
	{
		UE_LOG(LogDZ, Warning, TEXT("GameInstance has no UIConfig, ability slots will be empty"));
		return;
	}
	
	if (UIConfig->AbilitySlots.Num() == 0)
	{
		UE_LOG(LogDZ, Warning, TEXT("UIConfig has no AbilitySlots, ability slots will be empty"));
		return;
	}
	
	SlotSkillIds.Reset();
	for (const FDZ_AbilitySlotLayout& SlotLayout : UIConfig->AbilitySlots)
	{
		SlotSkillIds.Add(SlotLayout.DefaultSkillId);
	}
}

int32 UDZ_GameInstance::GetSlotSkillId(int32 SlotIndex) const
{
	return SlotSkillIds.IsValidIndex(SlotIndex) ? SlotSkillIds[SlotIndex] : DZSkill::EmptyId;
}

int32 UDZ_GameInstance::GetSlotSkillCount() const
{
	return SlotSkillIds.Num();
}

void UDZ_GameInstance::SetSlotSkillId(int32 SlotIndex, int32 SkillId)
{
	if (!SlotSkillIds.IsValidIndex(SlotIndex)) return;
	if (SlotSkillIds[SlotIndex] == SkillId) return;
	
	if (SkillId != DZSkill::EmptyId)
	{
		const int32 OldIndex = SlotSkillIds.Find(SkillId);
		if (OldIndex != INDEX_NONE)
		{
			SlotSkillIds[OldIndex] = SlotSkillIds[SlotIndex];
		}
	}
	
	SlotSkillIds[SlotIndex] = SkillId;
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

int32 UDZ_GameInstance::GetStashUsedSlots() const
{
	int32 UsedSlots = 0;
	for (const FDZ_ItemStack& Item : StashItems)
	{
		if (Item.IsEmpty()) continue;
		UsedSlots++;
	}
	return UsedSlots;
}

int32 UDZ_GameInstance::GetStashBagUsedSlots() const
{
	int32 UsedSlots = 0;
	for (const FDZ_ItemStack& Item : StashBagItems)
	{
		if (Item.IsEmpty()) continue;
		UsedSlots++;
	}
	return UsedSlots;
}

int32 UDZ_GameInstance::GetStashTotalValue()
{
	int32 TotalValue = 0;
	for (const FDZ_ItemStack& Item : StashItems)
	{
		if (Item.IsEmpty()) continue;
		const FDZ_ItemData* Data = GetItemData(Item.ItemId);
		if (!Data) continue;
		TotalValue += Data->Value * Item.Count;
	}
	return TotalValue;
}

bool UDZ_GameInstance::MoveStashItem(const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To, FText& OutFailReason)
{
	OutFailReason = FText::GetEmpty();
	
	if (From.Container == To.Container && From.SlotIndex == To.SlotIndex) return false;
	if (!IsStashSlotValid(From) || !IsStashSlotValid(To)) return false;
	
	const FDZ_ItemStack& Source = GetStashSlotItem(From);
	if (Source.IsEmpty()) return false;
	if (!CanAcceptStashItem(To, Source.ItemId, Source.Count))
	{
		OutFailReason = FText::FromString(TEXT("该物品不能放在这个槽位"));
		return false;
	}
	
	const FDZ_ItemStack& Target = GetStashSlotItem(To);
	if (!Target.IsEmpty() && !CanAcceptStashItem(From, Target.ItemId, Target.Count))
	{
		OutFailReason = FText::FromString(TEXT("目标物品无法放回原槽位"));
		return false;
	}
	
	if (Target.IsEmpty())
	{
		SetStashSlotItem(To, Source);
		ClearStashSlot(From);
		OnStashChanged.Broadcast();
		return true;
	}
	if (Target.ItemId == Source.ItemId)
	{
		if (To.Container == EItemContainer::LoadoutEquipment) return true;
		const int32 MaxStack = ItemHelper::GetMaxStack(this, Source.ItemId);
		if (Target.Count + Source.Count > MaxStack)
		{
			OutFailReason = FText::FromString(TEXT("堆叠已满"));
			return false;
		}
			
		SetStashSlotItem(To, FDZ_ItemStack(Source.ItemId, Target.Count + Source.Count));
		ClearStashSlot(From);
	}
	else
	{
		SetStashSlotItem(To, Source);
		SetStashSlotItem(From, Target);
	}
	
	OnStashChanged.Broadcast();
	return true;
}

const TArray<FDZ_ItemStack>* UDZ_GameInstance::GetStashStackArray(EItemContainer Container) const
{
	if (Container == EItemContainer::Stash) return &StashItems;
	if (Container == EItemContainer::LoadoutBag) return &StashBagItems;
	return nullptr;
}

TArray<FDZ_ItemStack>* UDZ_GameInstance::GetStashStackArray(EItemContainer Container)
{
	if (Container == EItemContainer::Stash) return &StashItems;
	if (Container == EItemContainer::LoadoutBag) return &StashBagItems;
	return nullptr;
}

bool UDZ_GameInstance::IsStashSlotValid(const FDZ_ItemSlotHandle& Handle) const
{
	if (Handle.Container == EItemContainer::LoadoutEquipment) return StashEquipments.IsValidIndex(Handle.SlotIndex);
	
	const TArray<FDZ_ItemStack>* Items = GetStashStackArray(Handle.Container);
	return Items && Items->IsValidIndex(Handle.SlotIndex);
}

FDZ_ItemStack UDZ_GameInstance::GetStashSlotItem(const FDZ_ItemSlotHandle& Handle) const
{
	if (Handle.Container == EItemContainer::LoadoutEquipment)
	{
		if (!StashEquipments.IsValidIndex(Handle.SlotIndex)) return FDZ_ItemStack();
		if (StashEquipments[Handle.SlotIndex] == DZItem::EmptyId) return FDZ_ItemStack();
		return FDZ_ItemStack(StashEquipments[Handle.SlotIndex], 1);
	}
	
	const TArray<FDZ_ItemStack>* Items = GetStashStackArray(Handle.Container);
	if (!Items || !Items->IsValidIndex(Handle.SlotIndex)) return FDZ_ItemStack();
	return (*Items)[Handle.SlotIndex];
}

void UDZ_GameInstance::SetStashSlotItem(const FDZ_ItemSlotHandle& Handle, const FDZ_ItemStack& Item)
{
	if (Handle.Container == EItemContainer::LoadoutEquipment)
	{
		if (!StashEquipments.IsValidIndex(Handle.SlotIndex)) return;
		StashEquipments[Handle.SlotIndex] = Item.ItemId;
		return;
	}
	
	TArray<FDZ_ItemStack>* Items = GetStashStackArray(Handle.Container);
	if (!Items || !Items->IsValidIndex(Handle.SlotIndex)) return;
	(*Items)[Handle.SlotIndex] = Item;
}

void UDZ_GameInstance::ClearStashSlot(const FDZ_ItemSlotHandle& Handle)
{
	SetStashSlotItem(Handle, FDZ_ItemStack());
}

bool UDZ_GameInstance::CanAcceptStashItem(const FDZ_ItemSlotHandle& Handle, int32 ItemId, int32 Count)
{
	if (Handle.Container != EItemContainer::LoadoutEquipment) return true;
	if (Count != 1) return false;
	
	const FDZ_ItemData* Data = GetItemData(ItemId);
	return Data && static_cast<int32>(Data->EquipSlot) == Handle.SlotIndex;
}
