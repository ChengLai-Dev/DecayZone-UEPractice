#include "Library/ItemHelper.h"

#include "Components/DZ_ContainerComponent.h"
#include "Components/DZ_InventoryComponent.h"
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
	// 背包->背包
	if (From.Container == EItemContainer::Backpack && To.Container == EItemContainer::Backpack)
	{
		return InventoryComp->MoveItem(From.SlotIndex, To.SlotIndex);
	}
	// 搜刮物品->背包
	if (From.Container == EItemContainer::Loot && To.Container == EItemContainer::Backpack)
	{
		if (!ContainerComp->IsSlotValid(From.SlotIndex)) return false;
		int32 ItemID = ContainerComp->GetItems()[From.SlotIndex].ItemId;
		int32 Count = ContainerComp->GetItems()[From.SlotIndex].Count;
		ContainerComp->RemoveItemsAll(From.SlotIndex);
		return InventoryComp->AddItem(To.SlotIndex, ItemID, Count);
	}
	return false;
}
