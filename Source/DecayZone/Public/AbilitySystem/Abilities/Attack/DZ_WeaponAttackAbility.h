#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/DZ_AttackAbility.h"
#include "DZ_WeaponAttackAbility.generated.h"

class UWeaponData;

UCLASS(Abstract)
class DECAYZONE_API UDZ_WeaponAttackAbility : public UDZ_AttackAbility
{
	GENERATED_BODY()

protected:
	virtual bool ResolveAttackData(FDZ_AttackData& OutAttackData) const override;

	const UWeaponData* ResolveWeaponData() const;
};
