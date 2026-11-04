// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DZ_PlayerController.generated.h"

class UDZ_UIConfig;
struct FInputActionValue;
class UDZ_InputConfig;
class UDZ_InventoryPanel;
/**
 * 
 */
UCLASS()
class DECAYZONE_API ADZ_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
private:
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void OpenOrCloseInventoryPanel();

	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UDZ_InputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UDZ_InventoryPanel> InventoryWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TObjectPtr<UDZ_UIConfig> UIConfig;
};
