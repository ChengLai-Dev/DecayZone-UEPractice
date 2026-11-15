// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DZ_RaidUIManager.h"

#include "UI/Controllers/DZ_AbilityBarController.h"
#include "UI/Controllers/DZ_InteractController.h"
#include "UI/Controllers/DZ_InventoryController.h"
#include "UI/Controllers/DZ_LootController.h"
#include "UI/Controllers/DZ_SettlementController.h"
#include "UI/Controllers/DZ_StatusBarsController.h"

void UDZ_RaidUIManager::CreateControllers()
{
	AbilityBarController = CreateController<UDZ_AbilityBarController>();
	StatusBarsController = CreateController<UDZ_StatusBarsController>();
	InteractController = CreateController<UDZ_InteractController>();
	InventoryController = CreateController<UDZ_InventoryController>();
	LootController = CreateController<UDZ_LootController>();
	SettlementController = CreateController<UDZ_SettlementController>();
}

void UDZ_RaidUIManager::DestroyControllers()
{
	Super::DestroyControllers();
	
	AbilityBarController = nullptr;
	StatusBarsController = nullptr;
	InteractController = nullptr;
	InventoryController = nullptr;
	LootController = nullptr;
	SettlementController = nullptr;
}
