#include "Library/ItemHelper.h"

#include "Components/DZ_ContainerComponent.h"
#include "Components/DZ_InventoryComponent.h"
#include "Data/Item/FDZ_ItemData.h"
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
		ContainerComp->RemoveItemsAll(From.SlotIndex);
		return InventoryComp->AddItem(To.SlotIndex, ItemID, Count);
	}
	// 搜刮物品->装备
	if (From.Container == EItemContainer::Loot && To.Container == EItemContainer::Equipment)
	{
		if (!ContainerComp || !ContainerComp->IsSlotValid(From.SlotIndex)) return false;
		if (!InventoryComp->IsEquipSlotValid(To.SlotIndex)) return false;
		int32 ItemID = ContainerComp->GetItems()[From.SlotIndex].ItemId;
		ContainerComp->RemoveItems(From.SlotIndex, 1);
		InventoryComp->SetEquipments(To.SlotIndex, ItemID);
		return true;
	}
	// 背包->搜刮物品
	if (From.Container == EItemContainer::Backpack && To.Container == EItemContainer::Loot)
	{
		if (!InventoryComp || !InventoryComp->IsSlotValid(From.SlotIndex)) return false;
		int32 ItemID = InventoryComp->GetItems()[From.SlotIndex].ItemId;
		int32 Count = InventoryComp->GetItems()[From.SlotIndex].Count;
		InventoryComp->RemoveItemsAll(From.SlotIndex);
		return ContainerComp->AddItem(ItemID, Count);
	}
	// 装备->搜刮物品
	if (From.Container == EItemContainer::Equipment && To.Container == EItemContainer::Loot)
	{
		if (!InventoryComp || !InventoryComp->IsEquipSlotValid(From.SlotIndex)) return false;
		int32 ItemID = InventoryComp->GetEquipments()[From.SlotIndex];
		if (ItemID == -1) return false;
		InventoryComp->RemoveItemsAll(From.SlotIndex);
		return ContainerComp->AddItem(ItemID, 1);
	}
	return false;
}
