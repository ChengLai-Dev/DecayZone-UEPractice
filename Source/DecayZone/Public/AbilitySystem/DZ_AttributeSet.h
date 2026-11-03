// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
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
	
private:
	
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);
	
};
