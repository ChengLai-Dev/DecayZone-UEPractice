// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/DZ_AttackAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayTags/DZGameplayTags.h"

void UDZ_AttackAbility::ApplyDamageEffectToTarget(AActor* Target)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, 1, ASC->MakeEffectContext());
		SpecHandle.Data->SetSetByCallerMagnitude(DZGameplayTags::SetByCaller_Damage, BaseDamage);
		ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Target, DZGameplayTags::Event_HitReact, FGameplayEventData());
	}
}
