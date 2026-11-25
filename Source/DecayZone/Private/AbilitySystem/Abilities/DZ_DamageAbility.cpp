// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DZ_DamageAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayTags/DZGameplayTags.h"

void UDZ_DamageAbility::ApplyDamageEffectToTarget(AActor* TargetActor, float Damage)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, 1, ASC->MakeEffectContext());
		SpecHandle.Data->SetSetByCallerMagnitude(DZGameplayTags::SetByCaller_Damage, Damage);
		ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, DZGameplayTags::Event_HitReact, FGameplayEventData());
	}
}
