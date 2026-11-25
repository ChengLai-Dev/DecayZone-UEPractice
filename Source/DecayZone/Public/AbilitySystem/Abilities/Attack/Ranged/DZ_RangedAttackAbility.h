#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/DZ_WeaponAttackAbility.h"
#include "DZ_RangedAttackAbility.generated.h"

class URangedWeaponData;

UCLASS()
class DECAYZONE_API UDZ_RangedAttackAbility : public UDZ_WeaponAttackAbility
{
	GENERATED_BODY()

protected:
	virtual bool ResolveAttackData(FDZ_AttackData& OutAttackData) const override;
	virtual void PerformAttack(const FDZ_AttackData& AttackData) override;

private:
	bool GetAimRay(FVector& OutStart, FVector& OutDirection, const URangedWeaponData& WeaponData) const;
};
