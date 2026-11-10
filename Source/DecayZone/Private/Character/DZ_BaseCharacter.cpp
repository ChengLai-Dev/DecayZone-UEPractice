// Fill out your copyright notice in the Description page of Project Settings.


#include "DecayZone/Public/Character/DZ_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Data/Ability/DZ_StartupAbilities.h"
#include "Game/DZ_GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Library/DZ_BlueprintLibrary.h"
#include "Library/ItemHelper.h"
#include "Net/UnrealNetwork.h"

ADZ_BaseCharacter::ADZ_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ADZ_BaseCharacter::CanInteract_Implementation()
{
	return bIsDead;
}

float ADZ_BaseCharacter::GetInteractDuration_Implementation()
{
	return InteractDuration;
}

FText ADZ_BaseCharacter::GetInteractPromptText_Implementation()
{
	return InteractPromptText;
}

void ADZ_BaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bIsDead);
}

UAbilitySystemComponent* ADZ_BaseCharacter::GetAbilitySystemComponent() const
{
	return nullptr;
}

void ADZ_BaseCharacter::UseItem(int32 ItemID)
{
	ItemHelper::UseItem(this, ItemID);
}

bool ADZ_BaseCharacter::IsFriendly(AActor* TargetActor)
{
	return UDZ_BlueprintLibrary::IsFriendly(this, TargetActor);
}

bool ADZ_BaseCharacter::IsAlive()
{
	return !bIsDead;
}

void ADZ_BaseCharacter::Death()
{
	if (!HasAuthority()) return;
	if (bIsDead) return;
	bIsDead = true;
	ClientHandleDeath();
}

void ADZ_BaseCharacter::ClientHandleDeath()
{
	OnDeathEvent.Broadcast();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void ADZ_BaseCharacter::BindDeathHandle()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (const UDZ_AttributeSet* AttributeSet = Cast<UDZ_AttributeSet>(ASC->GetAttributeSet(UDZ_AttributeSet::StaticClass())))
		{
			const_cast<UDZ_AttributeSet*>(AttributeSet)->OnDeath.AddDynamic(this, &ADZ_BaseCharacter::Death);
		}
	}
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
		}
		for (const TSubclassOf<UGameplayAbility>& Ability : Info.ActiveOnGivenAbilities)
		{
			GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability));
			GetAbilitySystemComponent()->TryActivateAbilityByClass(Ability);
		}
		return;
	}
}

void ADZ_BaseCharacter::OnRep_IsDead()
{
	ClientHandleDeath();
}
