// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTT_TryActivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UBTT_TryActivateAbility : public UBTTask_BlueprintBase
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, Category = "DecayZone")
	FGameplayTag AbilityTag;
};
