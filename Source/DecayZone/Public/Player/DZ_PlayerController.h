// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DZ_PlayerController.generated.h"

struct FInputActionValue;
class UDZ_InputConfig;
/**
 * 
 */
UCLASS()
class DECAYZONE_API ADZ_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetupInputComponent() override;
	
private:
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UDZ_InputConfig> InputConfig;
	
};
