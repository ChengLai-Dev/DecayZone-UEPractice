// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UIManager.generated.h"

class UDZ_UserWidget;
class UDZ_AbilityBarController;
class UDZ_StatusBarsController;
class UDZ_InteractController;
class UDZ_InventoryController;
class UDZ_LootController;
class UDZ_SettlementController;
class UDZ_UIControllerBase;
class ADZ_PlayerController;

UCLASS()
class DECAYZONE_API UUIManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(ADZ_PlayerController* InPlayerController);
	void Shutdown();
	void TickControllers(float DeltaSeconds);
	
	ADZ_PlayerController* GetOwningPlayerController() const { return PlayerController.Get(); }
	bool IsInitialized() const { return Controllers.Num() > 0; }
	
	void OpenWidget(UDZ_UserWidget* Widget, bool bResident=false);
	void CloseWidget(UDZ_UserWidget* Widget);
	void CloseTopWidget();
	void UpdateInputMode();
	
	UPROPERTY()
	TObjectPtr<UDZ_AbilityBarController> AbilityBarController;
	
	UPROPERTY()
	TObjectPtr<UDZ_StatusBarsController> StatusBarsController;
	
	UPROPERTY()
	TObjectPtr<UDZ_InteractController> InteractController;
	
	UPROPERTY()
	TObjectPtr<UDZ_InventoryController> InventoryController;
	
	UPROPERTY()
	TObjectPtr<UDZ_LootController> LootController;
	
	UPROPERTY()
	TObjectPtr<UDZ_SettlementController> SettlementController;
	
private:
	void CreateControllers();
	void DestroyControllers();
	
	TWeakObjectPtr<ADZ_PlayerController> PlayerController;
	
	TArray<TWeakObjectPtr<UDZ_UserWidget>> CurrentActivateWidgets;
	
	UPROPERTY()
	TArray<TObjectPtr<UDZ_UIControllerBase>> Controllers;
};
