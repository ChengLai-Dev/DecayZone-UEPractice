#pragma once

#include "CoreMinimal.h"
#include "Data/ItemType/Weapon/DZ_WeaponData.h"
#include "DZ_RangedWeaponData.generated.h"

UCLASS()
class DECAYZONE_API URangedWeaponData : public UWeaponData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	FName MuzzleSocketName = TEXT("Muzzle");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	float SpreadAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Ranged")
	bool bAutomatic = false;
};
