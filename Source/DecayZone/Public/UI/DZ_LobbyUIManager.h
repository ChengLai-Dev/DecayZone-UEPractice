// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/UIManager.h"
#include "DZ_LobbyUIManager.generated.h"

class UDZ_ShopController;
class UDZ_StashController;

UCLASS()
class DECAYZONE_API UDZ_LobbyUIManager : public UUIManager
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TObjectPtr<UDZ_StashController> StashController;

	UPROPERTY()
	TObjectPtr<UDZ_ShopController> ShopController;

protected:
	virtual void CreateControllers() override;
	virtual void DestroyControllers() override;
};
