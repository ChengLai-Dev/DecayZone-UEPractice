// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DZ_AbilitySystemComponent.h"

UDZ_AbilitySystemComponent::UDZ_AbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

FGameplayAbilitySpec* UDZ_AbilitySystemComponent::FindAbilitySpecByTag(const FGameplayTag& AbilityTag)
{
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.Ability && Spec.Ability->GetAssetTags().HasTag(AbilityTag))
		{
			return &Spec;
		}
	}
	return nullptr;
}