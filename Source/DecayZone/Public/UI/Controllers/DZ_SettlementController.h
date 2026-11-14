// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_SettlementController.generated.h"

class UDZ_SettlementPanel;
struct FDZ_SettlementInfo;

UCLASS()
class DECAYZONE_API UDZ_SettlementController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(ADZ_PlayerController* InPlayerController) override;
	virtual void Shutdown() override;

	void Show(const FDZ_SettlementInfo& InSettlementInfo);

private:
	UPROPERTY()
	TObjectPtr<UDZ_SettlementPanel> SettlementPanelWidget;
};
