// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_SettlementController.h"

#include "Blueprint/UserWidget.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Game/DZ_GameInstance.h"
#include "Player/DZ_RaidPlayerController.h"
#include "UI/Widgets/Settlement/DZ_SettlementPanel.h"

void UDZ_SettlementController::Initialize(APlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig)) return;
	
	RaidPlayerController = Cast<ADZ_RaidPlayerController>(PlayerController);
	if (!IsValid(RaidPlayerController)) return;

	SettlementPanelWidget = CreateWidget<UDZ_SettlementPanel>(RaidPlayerController, UIConfig->SettlementPanelClass);
	if (!IsValid(SettlementPanelWidget)) return;

	SettlementPanelWidget->OnConfirmRequested.AddDynamic(this, &UDZ_SettlementController::HandleConfirmRequested);
}

void UDZ_SettlementController::Shutdown()
{
	if (IsValid(SettlementPanelWidget))
	{
		SettlementPanelWidget->OnConfirmRequested.RemoveAll(this);
		SettlementPanelWidget->RemoveFromParent();
	}
	SettlementPanelWidget = nullptr;
	RaidPlayerController = nullptr;
	bHasSettlementInfo = false;
	Super::Shutdown();
}

void UDZ_SettlementController::Show(const FDZ_SettlementInfo& InSettlementInfo)
{
	if (!IsValid(SettlementPanelWidget)) return;

	CurrentSettlementInfo = InSettlementInfo;
	bHasSettlementInfo = true;

	SettlementPanelWidget->SetSettlementInfo(InSettlementInfo);
	SettlementPanelWidget->Open();
}

void UDZ_SettlementController::HandleConfirmRequested()
{
	if (!bHasSettlementInfo) return;
	bHasSettlementInfo = false;

	if (CurrentSettlementInfo.bSucceeded && IsValid(GameInstance))
	{
		GameInstance->DepositRaidResult(CurrentSettlementInfo.Items, CurrentSettlementInfo.Equipments);
	}

	if (IsValid(RaidPlayerController))
	{
		RaidPlayerController->ReturnToLobby();
	}
}
