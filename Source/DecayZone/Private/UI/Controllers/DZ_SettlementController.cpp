// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_SettlementController.h"

#include "Blueprint/UserWidget.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Data/UI/FDZ_SettlementInfo.h"
#include "Player/DZ_PlayerController.h"
#include "UI/Widgets/Settlement/DZ_SettlementPanel.h"

void UDZ_SettlementController::Initialize(ADZ_PlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig)) return;

	SettlementPanelWidget = CreateWidget<UDZ_SettlementPanel>(PlayerController, UIConfig->SettlementPanelClass);
}

void UDZ_SettlementController::Shutdown()
{
	if (IsValid(SettlementPanelWidget))
	{
		SettlementPanelWidget->RemoveFromParent();
	}
	SettlementPanelWidget = nullptr;
	Super::Shutdown();
}

void UDZ_SettlementController::Show(const FDZ_SettlementInfo& InSettlementInfo)
{
	if (!IsValid(SettlementPanelWidget)) return;

	SettlementPanelWidget->SetSettlementInfo(InSettlementInfo);
	SettlementPanelWidget->Open();
}
