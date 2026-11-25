#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/DZ_WeaponAttack.h"
#include "DZ_MeleeWeaponAttack.generated.h"

UCLASS()
class DECAYZONE_API UDZ_MeleeWeaponAttack : public UDZ_WeaponAttack
{
	GENERATED_BODY()

protected:
	virtual bool ResolveAttackData(FDZ_AttackData& OutAttackData) const override;
	virtual void PerformAttack(const FDZ_AttackData& AttackData) override;
};
