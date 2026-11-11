// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DZ_BaseEnemyController.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionComponent.h"

#include "AbilitySystemComponent.h"
#include "Character/DZ_BaseCharacter.h"
#include "GameplayTags/DZGameplayTags.h"

ADZ_BaseEnemyController::ADZ_BaseEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComp");
	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ThisClass::HandlePerceptionUpdate);
}

void ADZ_BaseEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InPawn);
	if (!ASC) return;
	
	MovementLockTagHandle = ASC->RegisterGameplayTagEvent(DZGameplayTags::State_MovementLocked, EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &ThisClass::HandleMovementLockedTagChanged);
}

void ADZ_BaseEnemyController::OnUnPossess()
{
	if (MovementLockTagHandle.IsValid())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()))
		{
			ASC->UnregisterGameplayTagEvent(MovementLockTagHandle, DZGameplayTags::State_MovementLocked, EGameplayTagEventType::NewOrRemoved);
		}
	}
	Super::OnUnPossess();
}

void ADZ_BaseEnemyController::HandleMovementLockedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount <= 0) return;
	
	StopMovement();
}

void ADZ_BaseEnemyController::HandlePerceptionUpdate(const TArray<AActor*>& Actors)
{
	EEnemyState AIState;
	if (Actors.IsEmpty())
	{
		AIState = EEnemyState::Patrol;
	}
	else
	{
		AIState = EEnemyState::Attack;
	}
	// GetBlackboardComponent()->SetValueAsEnum(FName("State"), AIState);
}
