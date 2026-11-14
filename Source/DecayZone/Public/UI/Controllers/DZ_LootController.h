// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_LootController.generated.h"

class UDZ_LootPanel;

UCLASS()
class DECAYZONE_API UDZ_LootController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(ADZ_PlayerController* InPlayerController) override;
	virtual void Shutdown() override;

	void Open();
	void Refresh(bool bForceRefresh);

private:
	UFUNCTION()
	void HandleItemMoveRequested(UWidget* From, UWidget* To);

	UFUNCTION()
	void HandleTakeItemRequested(UWidget* Row);

	UPROPERTY()
	TObjectPtr<UDZ_LootPanel> LootPanelWidget;
};
