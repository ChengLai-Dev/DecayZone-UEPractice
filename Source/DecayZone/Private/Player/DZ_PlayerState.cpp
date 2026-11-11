// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_PlayerState.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"

ADZ_PlayerState::ADZ_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDZ_AbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UDZ_AttributeSet>("AttributeSet");
	
	AbilitySystemComponent->SetIsReplicated(true);
}

UAbilitySystemComponent* ADZ_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UDZ_AttributeSet* ADZ_PlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
