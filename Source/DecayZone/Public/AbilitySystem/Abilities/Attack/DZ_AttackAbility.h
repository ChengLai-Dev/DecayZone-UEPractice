#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DZ_DamageAbility.h"
#include "Data/Item/FDZ_AttackData.h"
#include "DZ_AttackAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
class UAbilityTask_WaitGameplayEvent;

UCLASS(Abstract)
class DECAYZONE_API UDZ_AttackAbility : public UDZ_DamageAbility
{
	GENERATED_BODY()

public:
	UDZ_AttackAbility();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool ResolveAttackData(FDZ_AttackData& OutAttackData) const { return false; }
	virtual void PerformAttack(const FDZ_AttackData& AttackData) {}

	void ApplyAttackDamage(AActor* TargetActor, const FDZ_AttackData& AttackData);

private:
	UFUNCTION()
	void HandleMontageCompleted();

	UFUNCTION()
	void HandleMontageInterrupted();

	UFUNCTION()
	void HandleMontageCancelled();

	UFUNCTION()
	void HandleAttackEvent(FGameplayEventData Payload);

	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitGameplayEvent> AttackEventTask;

	UPROPERTY()
	FDZ_AttackData CachedAttackData;
};
