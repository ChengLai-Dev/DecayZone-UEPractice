#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/DZ_AttackAbility.h"
#include "DZ_WeaponAttack.generated.h"

class UWeaponData;

UCLASS(Abstract)
class DECAYZONE_API UDZ_WeaponAttack : public UDZ_AttackAbility
{
	GENERATED_BODY()

protected:
	virtual bool ResolveAttackData(FDZ_AttackData& OutAttackData) const override;

	const UWeaponData* ResolveWeaponData() const;
};
