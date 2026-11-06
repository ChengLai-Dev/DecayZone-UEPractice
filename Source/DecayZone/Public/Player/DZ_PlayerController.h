// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DZ_PlayerController.generated.h"

class UDZ_UserWidget;
class UDZ_LootPanel;
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
	
	UFUNCTION(Server, Reliable)
	void ServerMoveItem(UDZ_InventoryComponent* InventoryComp, UDZ_ContainerComponent* ContainerComp, const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To);
	
	UFUNCTION(Client, Reliable)
	void ClientInteractFinished(AActor* Target, bool bSucceeded);

private:
	UFUNCTION()
	void HandleItemMoveRequested(UWidget* From, UWidget* To);
	
	UFUNCTION()
	void HandleInventoryItemsChanged();
	
	void FinishInteract();
	void ActivateAbilityByTag(const FGameplayTag& AbilityTag);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void CloseTopWidget();
	void OpenOrCloseInventoryPanel();

	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UDZ_InputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UDZ_InventoryPanel> InventoryPanelWidget;
	
	UPROPERTY()
	TObjectPtr<UDZ_InteractPrompt> InteractPromptWidget;
	
	UPROPERTY()
	TObjectPtr<UDZ_InteractProgress> InteractProgressWidget;
	
	UPROPERTY()
	TObjectPtr<UDZ_LootPanel> LootPanelWidget;
	
	float TotalDuration = 0.f;
	float ElapsedTime = 0.f;
	
	bool bIsInteracting = false;
	
	FTimerHandle InteractTimerHandle;
	TWeakObjectPtr<AActor> InteractTarget;
};

