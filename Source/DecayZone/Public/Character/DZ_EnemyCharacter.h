// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DZ_BaseCharacter.h"
#include "GameplayEffect.h"
#include "DZ_EnemyCharacter.generated.h"

class ADZ_PatrolRoute;
struct FGameplayTag;
class UDZ_AttributeSet;
class UDZ_ContainerComponent;
class UDZ_AbilitySystemComponent;

UCLASS()
class DECAYZONE_API ADZ_EnemyCharacter : public ADZ_BaseCharacter
{
	GENERATED_BODY()

public:
	ADZ_EnemyCharacter();
	bool HasPatrolRoute();
	
	UFUNCTION(BlueprintCallable)
	FVector GetNextPatrolLocation();
	
protected:
	virtual void OnAbilitySystemInitialized() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UPROPERTY()
	TObjectPtr<UDZ_AttributeSet> AttributeSet;
	
	virtual void ClientHandleDeath() override;
	
private:
	void HandleMovementLockedTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	UPROPERTY(EditAnywhere, Category = "DecayZone|AI")
	TObjectPtr<ADZ_PatrolRoute> PatrolRoute;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TObjectPtr<UDZ_ContainerComponent> ContainerComponent;
	
	FDelegateHandle MovementLockTagHandle;
	
	FRotator BaseRotationRate = FRotator::ZeroRotator;
};
