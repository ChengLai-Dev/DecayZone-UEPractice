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
		SetEndReason(EDZAbilityEndReason::Failed, TEXT("NoEventData"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	// const FGameplayEffectSpecHandle& SpecHandle = ASC->MakeOutgoingSpec(InteractEffectClass, 1, ASC->MakeEffectContext());
	// InteractEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	
	InteractTarget = const_cast<AActor*>(TriggerEventData->Target.Get());
	if (!InteractTarget->Implements<UDZ_Interactable>() || !IDZ_Interactable::Execute_CanInteract(InteractTarget))
	{
		SetEndReason(EDZAbilityEndReason::Failed, TEXT("TargetInvalid"));
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
		SetEndReason(EDZAbilityEndReason::Failed, TEXT("TaskFailed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	WaitDelayTask->OnFinish.AddDynamic(this, &ThisClass::OnInteractTimerFinished);
	WaitDelayTask->ReadyForActivation();
}

void UDZ_InteractAbility::OnAbilityEnded(EDZAbilityEndReason Reason)
{
	const bool bSucceeded = (Reason == EDZAbilityEndReason::Completed);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (InteractEffectHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(InteractEffectHandle);
			InteractEffectHandle.Invalidate();
		}
	}

	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (ADZ_PlayerController* PlayerController = Cast<ADZ_PlayerController>(ActorInfo->PlayerController.Get()))
		{
			PlayerController->NotifyInteractFinished(InteractTarget.Get(), bSucceeded);
		}
	}
	if (IsValid(InteractTarget) && InteractTarget->Implements<UDZ_Interactable>())
	{
		IDZ_Interactable::Execute_OnInteractCompleted(InteractTarget.Get(), GetAvatarActorFromActorInfo(), bSucceeded);
	}
	InteractTarget = nullptr;
	WaitDelayTask = nullptr;
}

void UDZ_InteractAbility::OnInteractTimerFinished()
{
	if (IsInteractValid())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	SetEndReason(EDZAbilityEndReason::Failed, TEXT("InteractInvalid"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
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
