// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "Player/DZ_ExtractResult.h"
#include "DZ_PlayerState.generated.h"

class UDZ_AttributeSet;
class UDZ_AbilitySystemComponent;
/**
 * 
 */
UCLASS()
class DECAYZONE_API ADZ_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ADZ_PlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UDZ_AttributeSet* GetAttributeSet() const;
	
	void SetExtractResult(EDZExtractResult Result);
	
	UFUNCTION(BlueprintPure, Category = "DecayZone|PlayerState")
	EDZExtractResult GetExtractResult() const { return ExtractResult; }
	
	void IncreasePlayerKills();
	void IncreaseEnemyKills();
	
	UFUNCTION(BlueprintPure, Category = "DecayZone|PlayerState")
	int32 GetEnemyKills() const { return EnemyKills; }
	
	UFUNCTION(BlueprintPure, Category = "DecayZone|PlayerState")
	int32 GetPlayerKills() const { return PlayerKills; }
	
private:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UDZ_AttributeSet> AttributeSet;
	
	UPROPERTY(Replicated)
	int32 EnemyKills = 0;
	
	UPROPERTY(Replicated)
	int32 PlayerKills = 0;
	
	UPROPERTY(Replicated)
	EDZExtractResult ExtractResult = EDZExtractResult::InProgress;
};
