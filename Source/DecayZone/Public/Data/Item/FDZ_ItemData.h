#pragma once

#include "CoreMinimal.h"
#include "DZ_ItemDefines.h"
#include "Engine/DataTable.h"
#include "FDZ_ItemSlotHandle.h"
#include "FDZ_ItemData.generated.h"

class UItemTypeData;
class UWeaponData;

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None,
	Heal,
	Throw,
	Equip,
	Buff
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Normal,
	Weapon,
	Armor,
};

USTRUCT(BlueprintType)
struct FDZ_ItemData: public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemId = DZItem::EmptyId;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Value;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UItemTypeData> Extension;

	EItemType GetItemType() const;
	EEquipSlot GetEquipSlot() const;
	float GetArmorBonus() const;
	const UWeaponData* GetWeaponData() const;
};
