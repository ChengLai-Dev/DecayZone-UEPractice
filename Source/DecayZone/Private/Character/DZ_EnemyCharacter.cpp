// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DZ_EnemyCharacter.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Actor/DZ_PatrolRoute.h"
#include "Components/DZ_ContainerComponent.h"
#include "GameplayTags/DZGameplayTags.h"


ADZ_EnemyCharacter::ADZ_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UDZ_AbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UDZ_AttributeSet>("AttributeSet");
	ContainerComponent = CreateDefaultSubobject<UDZ_ContainerComponent>("ContainerComponent");
	
	AbilitySystemComponent->SetIsReplicated(true);
}

UAbilitySystemComponent* ADZ_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

bool ADZ_EnemyCharacter::HasPatrolRoute()
{
	return IsValid(PatrolRoute);
}

FVector ADZ_EnemyCharacter::GetNextPatrolLocation()
{
	return PatrolRoute->GetNextPatrolPoint();
}

void ADZ_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	if (HasAuthority())
	{
		const FGameplayEffectSpecHandle& SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InitializeGEClass, 1, GetAbilitySystemComponent()->MakeEffectContext());
		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		GiveStartupAbilities();
		BindDeathHandle();
	}
}

void ADZ_EnemyCharacter::ClientHandleDeath()
{
	Super::ClientHandleDeath();
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
}

