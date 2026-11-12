// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "DZ_AbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DECAYZONE_API UDZ_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	UDZ_AbilitySystemComponent();

	FGameplayAbilitySpec* FindAbilitySpecByTag(const FGameplayTag& AbilityTag);

};
