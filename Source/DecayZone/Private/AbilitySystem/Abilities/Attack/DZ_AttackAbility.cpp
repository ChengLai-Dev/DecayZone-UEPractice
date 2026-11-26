#include "AbilitySystem/Abilities/Attack/DZ_AttackAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayTags/DZGameplayTags.h"

UDZ_AttackAbility::UDZ_AttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UDZ_AttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	FDZ_AttackData AttackData;
	if (!ResolveAttackData(AttackData) || !IsValid(AttackData.AttackMontage))
	{
		SetEndReason(EDZAbilityEndReason::Failed, TEXT("NoAttackData"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedAttackData = AttackData;

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		SetEndReason(EDZAbilityEndReason::Failed, TEXT("CommitFailed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackData.AttackMontage);
	if (!MontageTask)
	{
		SetEndReason(EDZAbilityEndReason::Failed, TEXT("MontageTaskFailed"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandleMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandleMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandleMontageCancelled);
	MontageTask->ReadyForActivation();

	AttackEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, DZGameplayTags::Event_Attack, nullptr, false, false);
	if (!AttackEventTask) return;

	AttackEventTask->EventReceived.AddDynamic(this, &ThisClass::HandleAttackEvent);
	AttackEventTask->ReadyForActivation();
}

void UDZ_AttackAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CachedAttackData.AttackCooldown <= 0.f) return;
	if (!CooldownGameplayEffectClass) return;
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid()) return;

	FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(
		CooldownGameplayEffectClass, GetAbilityLevel(Handle, ActorInfo), Context);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(DZGameplayTags::SetByCaller_Cooldown, CachedAttackData.AttackCooldown);
	ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UDZ_AttackAbility::ApplyAttackDamage(AActor* TargetActor, const FDZ_AttackData& AttackData)
{
	if (!IsValid(TargetActor)) return;

	ApplyDamageEffectToTarget(TargetActor, AttackData.Damage);
}

void UDZ_AttackAbility::HandleMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UDZ_AttackAbility::HandleMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDZ_AttackAbility::HandleMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UDZ_AttackAbility::HandleAttackEvent(FGameplayEventData Payload)
{
	PerformAttack(CachedAttackData);
}
