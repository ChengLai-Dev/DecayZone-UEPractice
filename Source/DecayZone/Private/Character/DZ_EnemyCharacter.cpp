// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DZ_EnemyCharacter.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Actor/DZ_PatrolRoute.h"
#include "Components/DZ_ContainerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayTags/DZGameplayTags.h"


ADZ_EnemyCharacter::ADZ_EnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UDZ_AbilitySystemComponent>("AbilitySystemComponent");
	AttributeSet = CreateDefaultSubobject<UDZ_AttributeSet>("AttributeSet");
	ContainerComponent = CreateDefaultSubobject<UDZ_ContainerComponent>("ContainerComponent");
	
	AbilitySystemComponent->SetIsReplicated(true);
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
	
	SetupAbilitySystem(GetAbilitySystemComponent(), this);
}

void ADZ_EnemyCharacter::OnAbilitySystemInitialized()
{
	if (!HasAuthority()) return;
	if (!AbilitySystemComponent) return;
	
	BaseRotationRate = GetCharacterMovement()->RotationRate;
	
	MovementLockTagHandle = AbilitySystemComponent->RegisterGameplayTagEvent(DZGameplayTags::State_MovementLocked, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &ThisClass::HandleMovementLockedTagChanged);
}

void ADZ_EnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AbilitySystemComponent && MovementLockTagHandle.IsValid())
	{
		AbilitySystemComponent->UnregisterGameplayTagEvent(MovementLockTagHandle, DZGameplayTags::State_MovementLocked, EGameplayTagEventType::NewOrRemoved);
		MovementLockTagHandle.Reset();
	}
	
	Super::EndPlay(EndPlayReason);
}

void ADZ_EnemyCharacter::HandleMovementLockedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent) return;
	
	if (NewCount > 0)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->MaxWalkSpeed = 0.f;
		MovementComponent->RotationRate = FRotator::ZeroRotator;
	}
	else
	{
		float MoveSpeed = 0;
		if (AbilitySystemComponent)
		{
			MoveSpeed = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMoveSpeedAttribute());
		}
		if (MoveSpeed > 0.f)
		{
			MovementComponent->MaxWalkSpeed = MoveSpeed;
		}
		MovementComponent->RotationRate = BaseRotationRate;
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

