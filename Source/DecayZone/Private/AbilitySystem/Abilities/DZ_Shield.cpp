// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DZ_Shield.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "GameplayTags/DZGameplayTags.h"

UDZ_Shield::UDZ_Shield()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDZ_Shield::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;
	ASC->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetShieldAttribute()).AddUObject(this, &ThisClass::OnShieldValueChanged);
	
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	K2_AddGameplayCue(DZGameplayTags::GameplayCue_Ability_Shield, ContextHandle);
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(ShieldGEClass, 1, ContextHandle);
	SpecHandle.Data->SetSetByCallerMagnitude(DZGameplayTags::SetByCaller_Shield, ShieldValue);
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	UAbilityTask_WaitDelay* WaitDelay = UAbilityTask_WaitDelay::WaitDelay(this, ShieldDuration);
	WaitDelay->OnFinish.AddDynamic(this, &ThisClass::OnShieldTimerFinished);
	WaitDelay->ReadyForActivation();
}

void UDZ_Shield::OnShieldTimerFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDZ_Shield::OnAbilityEnded(EDZAbilityEndReason Reason)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetShieldAttribute()).RemoveAll(this);
		ASC->SetNumericAttributeBase(UDZ_AttributeSet::GetShieldAttribute(), 0);
	}
	Super::OnAbilityEnded(Reason);
}

void UDZ_Shield::OnShieldValueChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue > 0 && Data.NewValue <= 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

 