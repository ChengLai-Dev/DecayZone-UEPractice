// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DZ_PlayerController.generated.h"

class UDZ_InteractProgress;
class UDZ_InteractPrompt;
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
	void Interact();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleInteractableActorChanged(AActor* Actor);
	virtual void OnPossess(APawn* InPawn) override;
	
	
	UFUNCTION(Server, Reliable)
	void ServerTakeItem(AActor* ContainerActor, int32 SlotIndex, int32 Count);
	
private:
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void OpenOrCloseInventoryPanel();

	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UDZ_InputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UDZ_InventoryPanel> InventoryWidget;
	
	UPROPERTY()
	TObjectPtr<UDZ_InteractPrompt> InteractPromptWidget;
	
	UPROPERTY()
	TObjectPtr<UDZ_InteractProgress> InteractProgressWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TObjectPtr<UDZ_UIConfig> UIConfig;
};
