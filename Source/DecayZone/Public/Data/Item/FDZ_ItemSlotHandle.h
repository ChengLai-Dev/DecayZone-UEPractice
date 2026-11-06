#pragma once
#include "CoreMinimal.h"
#include "FDZ_ItemSlotHandle.generated.h"


UENUM(BlueprintType)
enum class EItemContainer: uint8
{
	Backpack,
	Equipment,
	Loot,
};

UENUM(BlueprintType)
enum class EEquipSlot: uint8
{
	Helmet,
	Armor,
	Primary,
	Secondary,
	Melee,
	Backpack,
	ChestRig,
	Headset,
};

USTRUCT(BlueprintType)
struct FDZ_ItemSlotHandle
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EItemContainer Container = EItemContainer::Backpack;
	
	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;
};
