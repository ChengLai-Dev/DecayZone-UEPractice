// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/DZ_GameplayAbility.h"
#include "DZ_DamageAbility.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_DamageAbility : public UDZ_GameplayAbility
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void ApplyDamageEffectToTarget(AActor* TargetActor, float Damage);
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Ability")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
