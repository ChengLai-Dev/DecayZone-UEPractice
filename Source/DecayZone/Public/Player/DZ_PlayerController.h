// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DZ_PlayerController.generated.h"

struct FGameplayTag;
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
	virtual void Tick(float DeltaSeconds) override;
	void Interact();
	void UpdateInteractProgress();
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleInteractableActorChanged(AActor* Actor);
	virtual void AcknowledgePossession(APawn* InPawn) override;

	UFUNCTION(Server, Reliable)
	void ServerTakeItem(AActor* ContainerActor, int32 SlotIndex, int32 Count);
	
	UFUNCTION(Server, Reliable)
	void ServerBeginInteract(AActor* ContainerActor);
	
	UFUNCTION(Client, Reliable)
	void ClientInteractFinished(AActor* Target);
	
private:
	void FinishInteract();
	void ActivateAbilityByTag(const FGameplayTag& AbilityTag);
	
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
	
	float TotalDuration = 0.f;
	float ElapsedTime = 0.f;
	
	bool bIsInteracting = false;
	
	FTimerHandle InteractTimerHandle;
	TWeakObjectPtr<AActor> InteractTarget;
};

