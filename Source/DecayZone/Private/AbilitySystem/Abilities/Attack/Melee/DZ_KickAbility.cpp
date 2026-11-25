#include "AbilitySystem/Abilities/Attack/Melee/DZ_KickAbility.h"

#include "Library/DZ_BlueprintLibrary.h"

bool UDZ_KickAbility::ResolveAttackData(FDZ_AttackData& OutAttackData) const
{
	OutAttackData = AttackData;
	return IsValid(AttackData.AttackMontage);
}

void UDZ_KickAbility::PerformAttack(const FDZ_AttackData& InAttackData)
{
	TArray<AActor*> HitActors;
	UDZ_BlueprintLibrary::MeleeTraceHit(GetAvatarActorFromActorInfo(), HitActors,
		InAttackData.Range, AttackAngle);

	for (AActor* HitActor : HitActors)
	{
		ApplyAttackDamage(HitActor, InAttackData);
	}
}
