#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace DZGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Kick);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Player_Attack_Weapon);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Enemy_Attack);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Armor);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_MovementLocked);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Interacting);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_HitReact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Attack_Melee);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Attack_Ranged);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Trigger_Interact);
	
}
