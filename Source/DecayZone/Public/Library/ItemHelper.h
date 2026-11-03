#pragma once

class ADZ_BaseCharacter;
struct FDZ_ItemData;

class ItemHelper
{
public:
	static const FDZ_ItemData* GetItemData(const UObject* WorldContextObject, int32 ItemID);
	
	static void UseItem(ADZ_BaseCharacter* Target, int32 ItemID);
};
