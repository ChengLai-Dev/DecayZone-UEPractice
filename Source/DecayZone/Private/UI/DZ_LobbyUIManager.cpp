// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DZ_LobbyUIManager.h"

#include "UI/Controllers/DZ_ShopController.h"
#include "UI/Controllers/DZ_StashController.h"

void UDZ_LobbyUIManager::CreateControllers()
{
	StashController = CreateController<UDZ_StashController>();
	ShopController = CreateController<UDZ_ShopController>();
}

void UDZ_LobbyUIManager::DestroyControllers()
{
	Super::DestroyControllers();
	
	StashController = nullptr;
	ShopController = nullptr;
}
