#pragma once

struct FDZ_ItemSlotHandle;
class UDZ_ContainerComponent;
class UDZ_InventoryComponent;
class ADZ_BaseCharacter;
struct FDZ_ItemData;

class ItemHelper
{
public:
	static const FDZ_ItemData* GetItemData(const UObject* WorldContextObject, int32 ItemID);
	
	static void UseItem(ADZ_BaseCharacter* Target, int32 ItemID);
	
	static bool MoveItem(UDZ_InventoryComponent* InventoryComp, UDZ_ContainerComponent* ContainerComp, 
		const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To);
};
