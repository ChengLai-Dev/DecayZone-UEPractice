// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "DZ_PlayerState.generated.h"

class UDZ_AttributeSet;
class UDZ_AbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DECAYZONE_API ADZ_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ADZ_PlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UDZ_AttributeSet* GetAttributeSet() const;
	
	void IncreasePlayerKills();
	void IncreaseEnemyKills();
	
private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UDZ_AttributeSet> AttributeSet;
	
	UPROPERTY()
	int32 EnemyKills = 0;
	
	UPROPERTY()
	int32 PlayerKills = 0;
};
