// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DZ_EnemyCharacter.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"


ADZ_EnemyCharacter::ADZ_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UDZ_AbilitySystemComponent>("AbilitySystemComponent");
}

UAbilitySystemComponent* ADZ_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADZ_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	if (HasAuthority())
	{
		const FGameplayEffectSpecHandle& SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeGEClass, 1, GetAbilitySystemComponent()->MakeEffectContext());
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		GiveStartupAbilities();
		BindDeathHandle();
	}
}

