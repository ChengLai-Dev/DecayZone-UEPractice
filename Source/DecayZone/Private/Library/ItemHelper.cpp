#include "Library/ItemHelper.h"

#include "Components/DZ_ContainerComponent.h"
#include "Components/DZ_InventoryComponent.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "Game/DZ_GameInstance.h"

const FDZ_ItemData* ItemHelper::GetItemData(const UObject* WorldContextObject, int32 ItemID)
{
	if (!IsValid(WorldContextObject)) return nullptr;
	
	UWorld* World = WorldContextObject->GetWorld();
	if (!IsValid(World)) return nullptr;
	
	UDZ_GameInstance* GI = World->GetGameInstance<UDZ_GameInstance>();
	if (!IsValid(GI)) return nullptr;
	
	return GI->GetItemData(ItemID);
}

void ItemHelper::UseItem(ADZ_BaseCharacter* Target, int32 ItemID)
{
	
}

bool ItemHelper::MoveItem(UDZ_InventoryComponent* InventoryComp, UDZ_ContainerComponent* ContainerComp,
	const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To)
{
	if (From.Container == To.Container && From.SlotIndex == To.SlotIndex) return false;
	// Inventory组件内move
	if ((From.Container == EItemContainer::Backpack || From.Container == EItemContainer::Equipment) && 
		(To.Container == EItemContainer::Backpack || To.Container == EItemContainer::Equipment))
	{
		return InventoryComp->MoveItem(From, To);
	}
	// 搜刮物品->背包
	if (From.Container == EItemContainer::Loot && To.Container == EItemContainer::Backpack)
	{
		if (!ContainerComp || !ContainerComp->IsSlotValid(From.SlotIndex)) return false;
		int32 ItemID = ContainerComp->GetItems()[From.SlotIndex].ItemId;
		int32 Count = ContainerComp->GetItems()[From.SlotIndex].Count;
		const FDZ_ItemData* Data = GetItemData(InventoryComp, ItemID);
		if (!Data || !InventoryComp->CanAddWeight(Data->Weight * Count)) return false;
		if (!InventoryComp->AddItem(To.SlotIndex, ItemID, Count)) return false;
		ContainerComp->RemoveItemsAll(From.SlotIndex);
		return true;
	}
	// 搜刮物品->装备
	if (From.Container == EItemContainer::Loot && To.Container == EItemContainer::Equipment)
	{
		if (!ContainerComp || !ContainerComp->IsSlotValid(From.SlotIndex)) return false;
		if (!InventoryComp->IsEquipSlotValid(To.SlotIndex)) return false;
		int32 ItemID = ContainerComp->GetItems()[From.SlotIndex].ItemId;
		const FDZ_ItemData* Data = GetItemData(InventoryComp, ItemID);
		if (!Data) return false;
		int32 EquipSlot = static_cast<int32>(Data->EquipSlot);
		if (EquipSlot != To.SlotIndex) return false;
		
		
		int32 OldItemID = InventoryComp->GetEquipments()[To.SlotIndex];
		if (OldItemID != DZItem::EmptyId)
		{
			const FDZ_ItemData* OldData = GetItemData(InventoryComp, OldItemID);
			if (!OldData || !InventoryComp->CanAddWeight(OldData->Weight)) return false;
		}
		ContainerComp->RemoveItems(From.SlotIndex, 1);
		InventoryComp->SetEquipments(To.SlotIndex, ItemID);
		if (OldItemID == DZItem::EmptyId) return true;
		if (!InventoryComp->AddItemAutoPos(OldItemID, 1))
		{
			ContainerComp->AddItem(OldItemID, 1);
		}
		return true;
	}
	// 背包->搜刮物品
	if (From.Container == EItemContainer::Backpack && To.Container == EItemContainer::Loot)
	{
		if (!InventoryComp || !InventoryComp->IsSlotValid(From.SlotIndex)) return false;
		if (!ContainerComp) return false;
		int32 ItemID = InventoryComp->GetItems()[From.SlotIndex].ItemId;
		int32 Count = InventoryComp->GetItems()[From.SlotIndex].Count;
		if (!ContainerComp->AddItem(ItemID, Count)) return false;
		InventoryComp->RemoveItemsAll(From.SlotIndex);
		return true;
	}
	// 装备->搜刮物品
	if (From.Container == EItemContainer::Equipment && To.Container == EItemContainer::Loot)
	{
		if (!InventoryComp || !InventoryComp->IsEquipSlotValid(From.SlotIndex)) return false;
		if (!ContainerComp) return false;
		int32 ItemID = InventoryComp->GetEquipments()[From.SlotIndex];
		if (ItemID == DZItem::EmptyId) return false;
		InventoryComp->ClearEquipment(From.SlotIndex);
		ContainerComp->AddItem(ItemID, 1);
		return true;
	}
	return false;
}

bool ItemHelper::TakeItem(UDZ_InventoryComponent* InventoryComp, UDZ_ContainerComponent* ContainerComp, int32 SlotIndex, int32 Count)
{
	if (!InventoryComp || !ContainerComp) return false;
	if (!ContainerComp->IsSlotValid(SlotIndex)) return false;
	
	const FDZ_ItemStack Stack = ContainerComp->GetItems()[SlotIndex];
	if (Stack.IsEmpty()) return false;
	
	const int32 TakeCount = FMath::Min(Count, Stack.Count);
	const FDZ_ItemData* Data = GetItemData(InventoryComp, Stack.ItemId);
	if (!Data || !InventoryComp->CanAddWeight(Data->Weight * TakeCount)) return false;
	
	if (!InventoryComp->AddItemAutoPos(Stack.ItemId, TakeCount)) return false;
	ContainerComp->RemoveItems(SlotIndex, TakeCount);
	return true;
}

int32 ItemHelper::GetMaxStack(const UObject* WorldContextObject, int32 ItemId)
{
	const FDZ_ItemData* Data = GetItemData(WorldContextObject, ItemId);
	if (!Data) return 1;
	if (Data->ItemType == EItemType::Equipment) return 1;
	
	return TNumericLimits<int32>::Max();
}
