// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_GameplayAbility.h"
#include "DZ_Shield.generated.h"

class AGameplayCueNotify_Actor;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_Shield : public UDZ_GameplayAbility
{
	GENERATED_BODY()
public:
	
	UDZ_Shield();
	
protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void OnAbilityEnded(EDZAbilityEndReason Reason) override;
	
private:
	UFUNCTION()
	void OnShieldTimerFinished();
	
	void OnShieldValueChanged(const FOnAttributeChangeData& Data);
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	float ShieldDuration = 8.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	float ShieldValue = 50.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TSubclassOf<UGameplayEffect> ShieldGEClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TSubclassOf<AGameplayCueNotify_Actor> ShieldGCClass;
};
