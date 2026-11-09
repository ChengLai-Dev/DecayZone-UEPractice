#pragma once

#include "CoreMinimal.h"
#include "DZ_ItemDefines.h"
#include "Engine/DataTable.h"
#include "FDZ_ItemSlotHandle.h"
#include "FDZ_WeaponParams.h"
#include "FDZ_ItemData.generated.h"

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
	Equipment,
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
	EItemType ItemType = EItemType::Normal;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEquipSlot EquipSlot = EEquipSlot::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ArmorBonus = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDZ_WeaponParams WeaponParams;
};
