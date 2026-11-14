// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_InventoryController.generated.h"

class UDZ_InventoryPanel;

UCLASS()
class DECAYZONE_API UDZ_InventoryController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(ADZ_PlayerController* InPlayerController) override;
	virtual void Shutdown() override;
	
	bool Toggle();
	void Refresh(bool bForceRefresh);

private:
	UFUNCTION()
	void HandleItemMoveRequested(UWidget* From, UWidget* To);

	UPROPERTY()
	TObjectPtr<UDZ_InventoryPanel> InventoryPanelWidget;
};
