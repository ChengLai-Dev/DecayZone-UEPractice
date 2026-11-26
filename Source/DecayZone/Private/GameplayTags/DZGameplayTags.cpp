#include "GameplayTags/DZGameplayTags.h"
#include "NativeGameplayTags.h"

namespace DZGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Ability_Interact, "Ability.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Kick, "Ability.Player.Attack.Kick");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Attack_Weapon, "Ability.Player.Attack.Weapon");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Skill_Shield, "Ability.Player.Skill.Shield");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Player_Skill_Sprint, "Ability.Player.Skill.Sprint");
	
	UE_DEFINE_GAMEPLAY_TAG(Ability_Enemy_Attack, "Ability.Enemy.Attack");
	
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Ability_Shield, "GameplayCue.Ability.Shield");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Fire, "GameplayCue.Weapon.Fire");
	UE_DEFINE_GAMEPLAY_TAG(GameplayCue_Weapon_Impact, "GameplayCue.Weapon.Impact");
	
	
	
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Armor, "SetByCaller.Armor");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Shield, "SetByCaller.Shield");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Cooldown, "SetByCaller.Cooldown");
	
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_Shield, "Cooldown.Skill.Shield");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_Sprint, "Cooldown.Skill.Sprint");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Attack, "Cooldown.Attack");
	
	UE_DEFINE_GAMEPLAY_TAG(State_MovementLocked, "State.MovementLocked");
	UE_DEFINE_GAMEPLAY_TAG(State_Interacting, "State.Interacting");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_HitReact, "Event.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Event_Attack, "Event.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Event_Attack_Melee, "Event.Attack.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Event_Attack_Ranged, "Event.Attack.Ranged");
	
	UE_DEFINE_GAMEPLAY_TAG(Event_Trigger_Interact, "Event.Trigger.Interact");
	
}
