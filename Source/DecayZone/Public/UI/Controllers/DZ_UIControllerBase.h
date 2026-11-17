// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DZ_UIControllerBase.generated.h"

class APlayerController;
class UDZ_GameInstance;
class UDZ_UIConfig;
class UUIManager;

UCLASS()
class DECAYZONE_API UDZ_UIControllerBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(APlayerController* InPlayerController);
	virtual void Shutdown();
	virtual void Tick(float DeltaSeconds);

protected:
	UUIManager* GetOwningManager() const;

	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<UDZ_GameInstance> GameInstance;

	UPROPERTY()
	TObjectPtr<UDZ_UIConfig> UIConfig;
};
