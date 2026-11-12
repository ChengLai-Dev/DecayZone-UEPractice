// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DZ_InteractAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/DZ_BaseCharacter.h"
#include "Character/DZ_PlayerCharacter.h"
#include "Interface/DZ_Interactable.h"
#include "Player/DZ_PlayerController.h"

UDZ_InteractAbility::UDZ_InteractAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UDZ_InteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!TriggerEventData || !IsValid(TriggerEventData->Target))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	// const FGameplayEffectSpecHandle& SpecHandle = ASC->MakeOutgoingSpec(InteractEffectClass, 1, ASC->MakeEffectContext());
	// InteractEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	InteractTarget = const_cast<AActor*>(TriggerEventData->Target.Get());
	if (!InteractTarget->Implements<UDZ_Interactable>() || !IDZ_Interactable::Execute_CanInteract(InteractTarget))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	float Duration = IDZ_Interactable::Execute_GetInteractDuration(InteractTarget);
	if (Duration <= 0.f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	WaitDelayTask = UAbilityTask_WaitDelay::WaitDelay(this, Duration);
	if (!WaitDelayTask)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnInteractTimerFinished);
	WaitDelayTask->ReadyForActivation();
}

void UDZ_InteractAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	const bool bSucceeded = !bWasCancelled;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (InteractEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(InteractEffectHandle);
			InteractEffectHandle.Invalidate();
		}
	}

	if (ActorInfo)
	{
		const bool bAutoInteract = IsValid(InteractTarget) && InteractTarget->Implements<UDZ_Interactable>() 
			&& IDZ_Interactable::Execute_IsAutoInteract(InteractTarget.Get());
		if (ADZ_PlayerController* PlayerController = Cast<ADZ_PlayerController>(ActorInfo->PlayerController.Get()))
		{
			if (bAutoInteract)
			{
				PlayerController->NotifyAutoInteractFinished(InteractTarget.Get(), bSucceeded);
			}
			else
			{
				PlayerController->NotifyInteractFinished(InteractTarget.Get(), bSucceeded);
			}
		}
	}
	if (IsValid(InteractTarget) && InteractTarget->Implements<UDZ_Interactable>())
	{
		if (bSucceeded)
		{
			IDZ_Interactable::Execute_OnInteractCompleted(InteractTarget.Get(), GetAvatarActorFromActorInfo());
		}
		else
		{
			IDZ_Interactable::Execute_OnInteractCancelled(InteractTarget.Get(), GetAvatarActorFromActorInfo());
		}
	}
	InteractTarget = nullptr;
	WaitDelayTask = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UDZ_InteractAbility::OnInteractTimerFinished()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, !IsInteractValid());
}

bool UDZ_InteractAbility::IsInteractValid() const
{
	if (!IsValid(InteractTarget)) return false;
	if (!InteractTarget->Implements<UDZ_Interactable>()) return false;
	if (!IDZ_Interactable::Execute_CanInteract(InteractTarget)) return false;
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetAvatarActorFromActorInfo());
	if (!PlayerCharacter || !PlayerCharacter->IsAlive()) return false;
	if (IDZ_Interactable::Execute_IsAutoInteract(InteractTarget))
	{
		return true;
	}
	return PlayerCharacter->IsInteractableInRange(InteractTarget);
}
