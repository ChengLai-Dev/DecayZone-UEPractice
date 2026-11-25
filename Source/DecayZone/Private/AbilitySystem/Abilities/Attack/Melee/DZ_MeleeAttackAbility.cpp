#include "AbilitySystem/Abilities/Attack/Melee/DZ_MeleeAttackAbility.h"

#include "Data/ItemType/Weapon/DZ_MeleeWeaponData.h"
#include "Library/DZ_BlueprintLibrary.h"

bool UDZ_MeleeAttackAbility::ResolveAttackData(FDZ_AttackData& OutAttackData) const
{
	const UMeleeWeaponData* MeleeWeaponData = Cast<UMeleeWeaponData>(ResolveWeaponData());
	if (!MeleeWeaponData) return false;

	OutAttackData = MeleeWeaponData->AttackData;
	return true;
}

void UDZ_MeleeAttackAbility::PerformAttack(const FDZ_AttackData& AttackData)
{
	const UMeleeWeaponData* MeleeWeaponData = Cast<UMeleeWeaponData>(ResolveWeaponData());
	if (!MeleeWeaponData) return;

	TArray<AActor*> HitActors;
	UDZ_BlueprintLibrary::MeleeTraceHit(GetAvatarActorFromActorInfo(), HitActors,
		AttackData.Range, MeleeWeaponData->AttackAngle);

	for (AActor* HitActor : HitActors)
	{
		ApplyAttackDamage(HitActor, AttackData);
	}
}
