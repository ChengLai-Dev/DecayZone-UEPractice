// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DZ_AttackAbility.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_AttackAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void ApplyDamageEffectToTarget(AActor* TargetActor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DecayZone|Ability")
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DecayZone|Ability")
	float BaseDamage = 20.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Ability")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
