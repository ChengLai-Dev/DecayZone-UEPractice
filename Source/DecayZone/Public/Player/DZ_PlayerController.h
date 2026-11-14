// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/PlayerController.h"
#include "DZ_PlayerController.generated.h"

struct FDZ_SettlementInfo;
struct FDZ_CharacterStatsInfo;
class UDZ_AbilitySystemComponent;
class UDZ_ContainerComponent;
class UDZ_InventoryComponent;
enum class EDZExtractResult : uint8;
struct FGameplayAttributeData;
struct FGameplayTag;
class UDZ_UIConfig;
struct FInputActionValue;
class UDZ_InputConfig;
class UUIManager;
/**
 * 
 */
UCLASS()
class DECAYZONE_API ADZ_PlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	virtual void Tick(float DeltaSeconds) override;
	
	void BeginInteract(AActor* Target);
	void CancelInteract();
	void EndInteract();

	void WeaponAttack();
	void Kick();
	
	const UDZ_InputConfig* GetInputConfig() const { return InputConfig; }
	
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;

	UUIManager* GetUIManager() const;
	
	virtual void OnRep_PlayerState() override;
	
	virtual void AcknowledgePossession(APawn* InPawn) override;

	UFUNCTION(Server, Reliable)
	void ServerTakeItem(AActor* ContainerActor, int32 SlotIndex, int32 Count);
	
	UFUNCTION(Server, Reliable)
	void ServerBeginInteract(AActor* ContainerActor);

	UFUNCTION(Server, Reliable)
	void ServerCancelInteract();
	
	UFUNCTION(Server, Reliable)
	void ServerMoveItem(UDZ_InventoryComponent* InventoryComp, UDZ_ContainerComponent* ContainerComp, const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To);
	
	UFUNCTION(Client, Reliable)
	void ClientInteractFinished(AActor* Target, bool bSucceeded);

	UFUNCTION(Client, Reliable)
	void ClientOpenLootPanel(UDZ_ContainerComponent* Container);
	
	UFUNCTION(BlueprintCallable, Category = "DecayZone|Extraction")
	void NotifyGameEnd(bool bExtracted);
	
	UFUNCTION(Client, Reliable)
	void ClientOpenSettlementPanel(const FDZ_SettlementInfo& SettlementInfo);
	
	void RefreshUIAll(bool bForceRefresh = true);
	void NotifyInteractFinished(AActor* Target, bool bSucceeded);
	
	UFUNCTION(BlueprintCallable, Category = "DecayZone|Ability")
	void SetSlotSkill(int32 SlotIndex, int32 SkillId);
	
	UFUNCTION(BlueprintPure, Category = "DecayZone|Ability")
	int32 GetSlotSkill(int32 SlotIndex) const;
	
	UDZ_InventoryComponent* GetInventoryComponent() const;
	bool BuildCharacterStats(FDZ_CharacterStatsInfo& OutStats) const;
	TWeakObjectPtr<UDZ_ContainerComponent> GetInteractContainer() const;

private:
	
	void Interact(AActor* InteractActor);
	void InteractHighlightActor();
	
	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);
	
	UFUNCTION()
	void RefreshStatusBars();

	UFUNCTION()
	void HandleInteractableActorChanged(AActor* Actor);
	
	UFUNCTION()
	void HandleInventoryItemsChanged();
	
	UFUNCTION()
	void HandleContainerItemsChanged();
	
	void ActivateAbilityByTag(const FGameplayTag& AbilityTag);
	
	bool IsMovementLocked() const;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void CloseTopWidget();
	void OpenOrCloseInventoryPanel();
	
	FDZ_SettlementInfo BuildSettlementInfo() const;
	void Settle(EDZExtractResult Result);

	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UDZ_InputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UUIManager> UIManager;
	
	TWeakObjectPtr<UDZ_ContainerComponent> InteractContainerComp;
	
	TWeakObjectPtr<UDZ_AbilitySystemComponent> AbilitySystemComponent;
};

