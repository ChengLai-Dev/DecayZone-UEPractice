#include "AbilitySystem/Abilities/Attack/Melee/DZ_NormalAttack.h"

#include "Library/DZ_BlueprintLibrary.h"

bool UDZ_NormalAttack::ResolveAttackData(FDZ_AttackData& OutAttackData) const
{
	OutAttackData = AttackData;
	return IsValid(AttackData.AttackMontage);
}

void UDZ_NormalAttack::PerformAttack(const FDZ_AttackData& InAttackData)
{
	TArray<AActor*> HitActors;
	UDZ_BlueprintLibrary::MeleeTraceHit(GetAvatarActorFromActorInfo(), HitActors,
		InAttackData.Range, AttackAngle);

	for (AActor* HitActor : HitActors)
	{
		ApplyAttackDamage(HitActor, InAttackData);
	}
}
