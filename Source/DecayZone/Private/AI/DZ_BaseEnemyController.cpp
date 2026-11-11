// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DZ_BaseEnemyController.h"
#include "Runtime/AIModule/Classes/Perception/AIPerceptionComponent.h"

ADZ_BaseEnemyController::ADZ_BaseEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("PerceptionComp");
	PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ThisClass::HandlePerceptionUpdate);
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
