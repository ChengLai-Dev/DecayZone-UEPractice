#include "AbilitySystem/Abilities/Attack/DZ_WeaponAttack.h"

#include "Character/DZ_PlayerCharacter.h"
#include "Data/ItemType/Weapon/DZ_WeaponData.h"

bool UDZ_WeaponAttack::ResolveAttackData(FDZ_AttackData& OutAttackData) const
{
	const UWeaponData* WeaponData = ResolveWeaponData();
	if (!WeaponData) return false;

	OutAttackData = WeaponData->AttackData;
	return true;
}

const UWeaponData* UDZ_WeaponAttack::ResolveWeaponData() const
{
	const ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetAvatarActorFromActorInfo());
	return PlayerCharacter ? PlayerCharacter->GetActiveWeaponData() : nullptr;
}
