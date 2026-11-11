// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTasks/BTT_TryActivateAbility.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_TryActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AIOwner->GetPawn());
	if (!ASC) return EBTNodeResult::Failed;
	
	if (ASC->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer())) return EBTNodeResult::Failed;
	return EBTNodeResult::Succeeded;
}
