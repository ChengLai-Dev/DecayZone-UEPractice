#include "AbilitySystem/Abilities/Attack/DZ_WeaponAttackAbility.h"

#include "Character/DZ_PlayerCharacter.h"
#include "Data/ItemType/Weapon/DZ_WeaponData.h"

bool UDZ_WeaponAttackAbility::ResolveAttackData(FDZ_AttackData& OutAttackData) const
{
	const UWeaponData* WeaponData = ResolveWeaponData();
	if (!WeaponData) return false;

	OutAttackData = WeaponData->AttackData;
	return true;
}

const UWeaponData* UDZ_WeaponAttackAbility::ResolveWeaponData() const
{
	const ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetAvatarActorFromActorInfo());
	return PlayerCharacter ? PlayerCharacter->GetActiveWeaponData() : nullptr;
}
