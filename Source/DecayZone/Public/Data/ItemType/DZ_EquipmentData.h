#pragma once

#include "CoreMinimal.h"
#include "Data/ItemType/DZ_ItemTypeData.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "DZ_EquipmentData.generated.h"

UCLASS(Abstract, BlueprintType)
class DECAYZONE_API UEquipmentData : public UItemTypeData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	EEquipSlot EquipSlot = EEquipSlot::None;
};
