// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UIManager.h"
#include "DZ_RaidUIManager.generated.h"

class UDZ_AbilityBarController;
class UDZ_StatusBarsController;
class UDZ_InteractController;
class UDZ_InventoryController;
class UDZ_LootController;
class UDZ_SettlementController;

UCLASS()
class DECAYZONE_API UDZ_RaidUIManager : public UUIManager
{
	GENERATED_BODY()

public:
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

protected:
	virtual void CreateControllers() override;
	virtual void DestroyControllers() override;
};
