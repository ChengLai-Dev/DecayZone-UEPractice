// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_PlayerState.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Net/UnrealNetwork.h"

ADZ_PlayerState::ADZ_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UDZ_AbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UDZ_AttributeSet>("AttributeSet");
	
	AbilitySystemComponent->SetIsReplicated(true);
}

void ADZ_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADZ_PlayerState, ExtractResult);
	DOREPLIFETIME(ADZ_PlayerState, EnemyKills);
	DOREPLIFETIME(ADZ_PlayerState, PlayerKills);
}

void ADZ_PlayerState::SetExtractResult(EDZExtractResult Result)
{
	ExtractResult = Result;
}

UAbilitySystemComponent* ADZ_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UDZ_AttributeSet* ADZ_PlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void ADZ_PlayerState::IncreasePlayerKills()
{
	if (!HasAuthority()) return;
	
	PlayerKills++;
}

void ADZ_PlayerState::IncreaseEnemyKills()
{
	if (!HasAuthority()) return;
	
	EnemyKills++;
}
