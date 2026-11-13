// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DZ_GameplayAbility.h"

#include "DecayZone/DecayZone.h"

void UDZ_GameplayAbility::SetEndReason(EDZAbilityEndReason Reason, FName Detail)
{
	EndReason = Reason;
	EndReasonDetail = Detail;
}

void UDZ_GameplayAbility::PreActivate(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	EndReason = EDZAbilityEndReason::Completed;
	EndReasonDetail = NAME_None;

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

EDZAbilityEndReason UDZ_GameplayAbility::GetEndReason(bool bWasCancelled) const
{
	if (!bWasCancelled) return EDZAbilityEndReason::Completed;

	return EndReason;
}

void UDZ_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		
		const EDZAbilityEndReason EndReason = GetEndReason(bWasCancelled);

		UE_LOG(LogDZ, Log, TEXT("[Ability] %s End. Reason=%s Detail=%s Cancelled=%d"),
			*GetNameSafe(this),
			*StaticEnum<EDZAbilityEndReason>()->GetNameStringByValue(static_cast<int64>(EndReason)),
			*EndReasonDetail.ToString(),
			bWasCancelled ? 1 : 0);

		OnAbilityEnded(EndReason);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
