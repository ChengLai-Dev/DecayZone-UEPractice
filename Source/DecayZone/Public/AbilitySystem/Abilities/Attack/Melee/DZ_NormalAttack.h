#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Attack/DZ_AttackAbility.h"
#include "DZ_NormalAttack.generated.h"

UCLASS()
class DECAYZONE_API UDZ_NormalAttack : public UDZ_AttackAbility
{
	GENERATED_BODY()

protected:
	virtual bool ResolveAttackData(FDZ_AttackData& OutAttackData) const override;
	virtual void PerformAttack(const FDZ_AttackData& InAttackData) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Attack")
	FDZ_AttackData AttackData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Attack")
	float AttackAngle = 120.f;
};
