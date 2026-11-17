// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_HUDController.h"

#include "Blueprint/UserWidget.h"
#include "Data/UI/DZ_UIConfig.h"
#include "GameFramework/PlayerController.h"
#include "UI/Controllers/DZ_AbilityBarController.h"
#include "UI/Controllers/DZ_RaidTimeController.h"
#include "UI/Controllers/DZ_StatusBarsController.h"
#include "UI/UIManager.h"
#include "UI/Widgets/HUD/DZ_HUDLayout.h"

void UDZ_HUDController::Initialize(APlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig)) return;
	if (!UIConfig->HUDClass) return;

	HUDLayout = CreateWidget<UDZ_HUDLayout>(PlayerController, UIConfig->HUDClass);
	if (!IsValid(HUDLayout)) return;

	UUIManager* Manager = GetOwningManager();
	if (!IsValid(Manager)) return;

	StatusBarsController = Manager->CreateController<UDZ_StatusBarsController>();
	AbilityBarController = Manager->CreateController<UDZ_AbilityBarController>();
	RaidTimeController = Manager->CreateController<UDZ_RaidTimeController>();

	StatusBarsController->SetStatusBarsWidget(HUDLayout->GetStatusBars());
	AbilityBarController->SetAbilityBarWidget(HUDLayout->GetAbilityBar());
	RaidTimeController->SetRaidTimeWidget(HUDLayout->GetRaidTime());

	HUDLayout->Open();
}

void UDZ_HUDController::Shutdown()
{
	if (IsValid(HUDLayout))
	{
		HUDLayout->RemoveFromParent();
	}

	HUDLayout = nullptr;
	StatusBarsController = nullptr;
	AbilityBarController = nullptr;
	RaidTimeController = nullptr;
	Super::Shutdown();
}

UDZ_HUDLayout* UDZ_HUDController::GetHUDLayout() const
{
	return HUDLayout;
}

void UDZ_HUDController::RefreshStatusBars()
{
	if (IsValid(StatusBarsController))
	{
		StatusBarsController->Refresh();
	}
}

void UDZ_HUDController::RefreshAbilityBar()
{
	if (IsValid(AbilityBarController))
	{
		AbilityBarController->Refresh();
	}
}

void UDZ_HUDController::HideAbilityBar()
{
	if (IsValid(AbilityBarController))
	{
		AbilityBarController->Hide();
	}
}
