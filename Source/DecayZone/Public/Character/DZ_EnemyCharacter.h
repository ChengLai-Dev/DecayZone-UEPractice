// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DZ_BaseCharacter.h"
#include "GameplayEffect.h"
#include "DZ_EnemyCharacter.generated.h"

class UDZ_AbilitySystemComponent;

UCLASS()
class DECAYZONE_API ADZ_EnemyCharacter : public ADZ_BaseCharacter
{
	GENERATED_BODY()

public:
	ADZ_EnemyCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UDZ_AbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TSubclassOf<UGameplayEffect> InitializeGEClass;
};
