// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DZ_GameplayAbility.h"
#include "DZ_InteractAbility.generated.h"

/**
 * 
 */
class UAbilityTask_WaitDelay;

UCLASS()
class DECAYZONE_API UDZ_InteractAbility : public UDZ_GameplayAbility
{
	GENERATED_BODY()
	
	
public:
	UDZ_InteractAbility();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnAbilityEnded(EDZAbilityEndReason Reason) override;

private:
	UFUNCTION()
	void OnInteractTimerFinished();
	bool IsInteractValid() const;

	UPROPERTY()
	TObjectPtr<AActor> InteractTarget;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitDelay> WaitDelayTask;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TSubclassOf<UGameplayEffect> InteractEffectClass;
	
	FActiveGameplayEffectHandle InteractEffectHandle;
};
