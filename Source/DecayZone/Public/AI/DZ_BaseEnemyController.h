// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "DZ_BaseEnemyController.generated.h"

struct FGameplayTag;

UENUM(BlueprintType)
enum class EEnemyState: uint8
{
	None,
	Patrol,
	Attack,
};

UCLASS()
class DECAYZONE_API ADZ_BaseEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	ADZ_BaseEnemyController();
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	
private:
	UFUNCTION()
	void HandlePerceptionUpdate(const TArray<AActor*>& Actors);
	
	void HandleMovementLockedTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	FDelegateHandle MovementLockTagHandle;
};
