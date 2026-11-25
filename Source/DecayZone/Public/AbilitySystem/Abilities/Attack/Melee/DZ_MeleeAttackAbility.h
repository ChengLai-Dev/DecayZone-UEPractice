#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/DZ_WeaponAttackAbility.h"
#include "DZ_MeleeAttackAbility.generated.h"

UCLASS()
class DECAYZONE_API UDZ_MeleeAttackAbility : public UDZ_WeaponAttackAbility
{
	GENERATED_BODY()

protected:
	virtual bool ResolveAttackData(FDZ_AttackData& OutAttackData) const override;
	virtual void PerformAttack(const FDZ_AttackData& AttackData) override;
};
