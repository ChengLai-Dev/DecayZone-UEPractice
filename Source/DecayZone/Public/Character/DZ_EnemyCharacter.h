// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DZ_BaseCharacter.h"
#include "GameplayEffect.h"
#include "DZ_EnemyCharacter.generated.h"

class ADZ_PatrolRoute;
class UDZ_AttributeSet;
class UDZ_ContainerComponent;
class UDZ_AbilitySystemComponent;

UCLASS()
class DECAYZONE_API ADZ_EnemyCharacter : public ADZ_BaseCharacter
{
	GENERATED_BODY()

public:
	ADZ_EnemyCharacter();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	bool HasPatrolRoute();
	
	UFUNCTION(BlueprintCallable)
	FVector GetNextPatrolLocation();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UDZ_AbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UDZ_AttributeSet> AttributeSet;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TSubclassOf<UGameplayEffect> InitializeGEClass;
	
	virtual void ClientHandleDeath() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "DecayZone|AI")
	TObjectPtr<ADZ_PatrolRoute> PatrolRoute;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TObjectPtr<UDZ_ContainerComponent> ContainerComponent;
};
