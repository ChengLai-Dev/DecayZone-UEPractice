// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/UI/FDZ_SettlementInfo.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_SettlementController.generated.h"

class ADZ_RaidPlayerController;
class UDZ_SettlementPanel;

UCLASS()
class DECAYZONE_API UDZ_SettlementController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(APlayerController* InPlayerController) override;
	virtual void Shutdown() override;

	void Show(const FDZ_SettlementInfo& InSettlementInfo);

private:
	UFUNCTION()
	void HandleConfirmRequested();

	UPROPERTY()
	TObjectPtr<UDZ_SettlementPanel> SettlementPanelWidget;
	
	UPROPERTY()
	TObjectPtr<ADZ_RaidPlayerController> RaidPlayerController;

	FDZ_SettlementInfo CurrentSettlementInfo;

	bool bHasSettlementInfo = false;
};
