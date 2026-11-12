// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTS_FindNearestSensedPlayer.h"

#include "AI/DZ_BaseEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/DZ_PlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"


UBTS_FindNearestSensedPlayer::UBTS_FindNearestSensedPlayer()
{
	bNotifyTick = true;
}

void UBTS_FindNearestSensedPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (!EnemyController.IsValid())
	{
		EnemyController = Cast<ADZ_BaseEnemyController>(AIOwner);
	}
	APawn* Pawn = AIOwner->GetPawn();
	if (!Pawn) return;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	TArray<AActor*> SensedActors;
	EnemyController->GetPerceptionComponent()->GetKnownPerceivedActors(nullptr, SensedActors);
	if (SensedActors.IsEmpty())
	{
		Blackboard->SetValueAsObject(AttackTarget.SelectedKeyName, nullptr);
		return;
	}
	
	AActor* MinDisActor = nullptr;
	float MinDistance = 0;
	
	for (AActor* Actor : SensedActors)
	{
		ADZ_PlayerCharacter* Player = Cast<ADZ_PlayerCharacter>(Actor);
		if (!Player || !Player->IsAlive()) continue;
		float Distance = FVector::Dist2D(Pawn->GetActorLocation(), Actor->GetActorLocation());
		if (!MinDisActor)
		{
			MinDisActor = Actor;
			MinDistance = Distance;
			continue;
		}
		
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			MinDisActor = Actor;
		}
	}
	Blackboard->SetValueAsObject(AttackTarget.SelectedKeyName, MinDisActor);
	// AActor* CurActor = Cast<AActor>(Blackboard->GetValueAsObject(AttackTarget.SelectedKeyName));
	// if (!CurActor)
	// {
	// 	Blackboard->SetValueAsObject(AttackTarget.SelectedKeyName, MinDisActor);
	// 	return;
	// }
	// if (CurActor == MinDisActor) return;
	// if (FVector::Dist2D(Pawn->GetActorLocation(), CurActor->GetActorLocation()) >= SwitchTargetRange)
	// {
	// 	Blackboard->SetValueAsObject(AttackTarget.SelectedKeyName, MinDisActor);
	// }
}
