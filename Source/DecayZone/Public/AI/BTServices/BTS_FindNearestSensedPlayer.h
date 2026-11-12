// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTS_FindNearestSensedPlayer.generated.h"

class ADZ_BaseEnemyController;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UBTS_FindNearestSensedPlayer : public UBTService_BlueprintBase
{
	GENERATED_BODY()
public:
	UBTS_FindNearestSensedPlayer();
protected:
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector AttackTarget;
	
private:
	
	TWeakObjectPtr<ADZ_BaseEnemyController> EnemyController;
	
	// UPROPERTY(EditAnywhere)
	// float SwitchTargetRange = 500;
};
