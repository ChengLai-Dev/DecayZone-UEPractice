// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DZ_UIControllerBase.generated.h"

class ADZ_PlayerController;
class UDZ_GameInstance;
class UDZ_UIConfig;

UCLASS()
class DECAYZONE_API UDZ_UIControllerBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(ADZ_PlayerController* InPlayerController);
	virtual void Shutdown();
	virtual void Tick(float DeltaSeconds);

protected:
	UPROPERTY()
	TObjectPtr<ADZ_PlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UDZ_GameInstance> GameInstance;

	UPROPERTY()
	TObjectPtr<UDZ_UIConfig> UIConfig;
};
