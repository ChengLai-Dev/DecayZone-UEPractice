// Fill out your copyright notice in the Description page of Project Settings.


#include "DecayZone/Public/Character/DZ_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "Data/Ability/DZ_StartupAbilities.h"
#include "Game/DZ_GameInstance.h"
#include "Library/ItemHelper.h"

ADZ_BaseCharacter::ADZ_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

UAbilitySystemComponent* ADZ_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ADZ_BaseCharacter::UseItem(int32 ItemID)
{
	ItemHelper::UseItem(this, ItemID);
}

void ADZ_BaseCharacter::GiveStartupAbilities()
{
	UDZ_GameInstance* GameInstance = Cast<UDZ_GameInstance>(GetGameInstance());
	for (const FStartupAbilityInfo& Info : GameInstance->StartupAbilities->StartupAbilityInfos)
	{
		if (GetClass() != Info.CharacterClass) continue;
		
		for (const TSubclassOf<UGameplayAbility>& Ability : Info.Abilities)
		{
			GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability));
			if (Info.bActivateOnGiven)
			{
				GetAbilitySystemComponent()->TryActivateAbilityByClass(Ability);
			}
		}
		return;
	}
}
