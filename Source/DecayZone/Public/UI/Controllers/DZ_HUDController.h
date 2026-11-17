// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_HUDController.generated.h"

class UDZ_AbilityBarController;
class UDZ_HUDLayout;
class UDZ_RaidTimeController;
class UDZ_StatusBarsController;

UCLASS()
class DECAYZONE_API UDZ_HUDController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(APlayerController* InPlayerController) override;
	virtual void Shutdown() override;

	UDZ_HUDLayout* GetHUDLayout() const;

	void RefreshStatusBars();
	void RefreshAbilityBar();
	void HideAbilityBar();

private:
	UPROPERTY()
	TObjectPtr<UDZ_HUDLayout> HUDLayout;

	UPROPERTY()
	TObjectPtr<UDZ_StatusBarsController> StatusBarsController;

	UPROPERTY()
	TObjectPtr<UDZ_AbilityBarController> AbilityBarController;

	UPROPERTY()
	TObjectPtr<UDZ_RaidTimeController> RaidTimeController;
};
