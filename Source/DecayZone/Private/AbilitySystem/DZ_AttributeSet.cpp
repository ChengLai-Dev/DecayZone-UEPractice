// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/DZ_AttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Character/DZ_EnemyCharacter.h"
#include "Character/DZ_PlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Player/DZ_PlayerState.h"

void UDZ_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Energy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxEnergy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Armor, COND_None, REPNOTIFY_Always);
}

void UDZ_AttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxEnergy());
	}
}

void UDZ_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
	}
	if (Attribute == GetEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxEnergy());
	}
}

void UDZ_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float OldHealth = GetHealth();
		const float Damage = GetIncomingDamage();
		const float DamageReduction = 1 - FMath::Clamp(GetArmor(), 0, 80) / 100;
		const float HealthValue = FMath::Clamp(GetHealth() - Damage * DamageReduction, 0, GetMaxHealth());
		SetIncomingDamage(0);
		SetHealth(HealthValue);
		if (OldHealth > 0 && HealthValue <= 0.0f)
		{
			OnDeath.Broadcast();
			const FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
			AActor* AttackOwner = Context.GetInstigator();
			AActor* ReceiveActor  = Data.Target.GetAvatarActor();

			if (ADZ_PlayerState* KillerState = Cast<ADZ_PlayerState>(AttackOwner))
			{
				if (Cast<ADZ_EnemyCharacter>(ReceiveActor))
				{
					KillerState->IncreaseEnemyKills();
				}
				else if (Cast<ADZ_PlayerCharacter>(ReceiveActor))
				{
					KillerState->IncreasePlayerKills();
				}
			}
		}
	}
}

void UDZ_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
}

void UDZ_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void UDZ_AttributeSet::OnRep_Energy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Energy, OldValue);
}

void UDZ_AttributeSet::OnRep_MaxEnergy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxEnergy, OldValue);
}

void UDZ_AttributeSet::OnRep_Armor(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Armor, OldValue);
}
