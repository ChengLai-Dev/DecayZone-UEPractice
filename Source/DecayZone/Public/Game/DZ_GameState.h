// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DZ_GameState.generated.h"

UCLASS()
class DECAYZONE_API ADZ_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADZ_GameState();
	
	UFUNCTION(BlueprintPure)
	float GetElapsedTime() const;
	
	UFUNCTION(BlueprintPure)
	float GetRunStartTime() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	
private:
	UPROPERTY(Replicated)
	float StartTime = 0;
};
