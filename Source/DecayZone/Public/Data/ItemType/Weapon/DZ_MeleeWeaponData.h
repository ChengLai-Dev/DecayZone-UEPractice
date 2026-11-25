#pragma once

#include "CoreMinimal.h"
#include "Data/ItemType/Weapon/DZ_WeaponData.h"
#include "DZ_MeleeWeaponData.generated.h"

UCLASS()
class DECAYZONE_API UMeleeWeaponData : public UWeaponData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Melee")
	float AttackAngle = 120.f;
};
