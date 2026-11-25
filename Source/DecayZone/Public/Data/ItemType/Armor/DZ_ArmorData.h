#pragma once

#include "CoreMinimal.h"
#include "Data/ItemType/DZ_EquipmentData.h"
#include "DZ_ArmorData.generated.h"

UCLASS()
class DECAYZONE_API UArmorData : public UEquipmentData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor")
	float ArmorBonus = 0.f;
};
