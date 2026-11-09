// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "DZ_GameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintCallable)
	void ApplyDamageEffectToTarget(AActor* TargetActor);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DecayZone|Ability")
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DecayZone|Ability")
	float BaseDamage;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Ability")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
