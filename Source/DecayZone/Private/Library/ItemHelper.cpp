#include "Library/ItemHelper.h"

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
