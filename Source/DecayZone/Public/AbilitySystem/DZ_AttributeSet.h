// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "DZ_DelegateTypes.h"
#include "DZ_AttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Health);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxHealth);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Energy);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, MaxEnergy);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, Armor);
	ATTRIBUTE_ACCESSORS_BASIC(ThisClass, IncomingDamage);
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnDeath;
	
private:
	
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_Energy)
	FGameplayAttributeData Energy;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxEnergy)
	FGameplayAttributeData MaxEnergy;
	
	UPROPERTY(ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	
	UPROPERTY()
	FGameplayAttributeData IncomingDamage;
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Energy(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxEnergy(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldValue);
};
