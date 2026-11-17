// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_RaidTimeController.h"

#include "DecayZone/DecayZone.h"
#include "Game/DZ_GameState.h"
#include "GameFramework/PlayerController.h"
#include "MVVMBlueprintLibrary.h"
#include "UI/ViewModel/DZ_RaidTimeViewModel.h"
#include "UI/Widgets/HUD/DZ_RaidTime.h"

void UDZ_RaidTimeController::SetRaidTimeWidget(UDZ_RaidTime* InWidget)
{
	RaidTimeWidget = InWidget;
	if (!IsValid(RaidTimeWidget))
	{
		UE_LOG(LogDZ, Warning, TEXT("HUD is missing the RaidTime child widget"));
		return;
	}

	ViewModel = NewObject<UDZ_RaidTimeViewModel>(this);
	UMVVMBlueprintLibrary::SetViewModelByClass(RaidTimeWidget, ViewModel.Get());
	RefreshTime();
}

void UDZ_RaidTimeController::Shutdown()
{
	ViewModel = nullptr;
	RaidTimeWidget = nullptr;
	Super::Shutdown();
}

void UDZ_RaidTimeController::Tick(float DeltaSeconds)
{
	RefreshTime();
}

void UDZ_RaidTimeController::RefreshTime()
{
	if (!IsValid(ViewModel) || !IsValid(PlayerController)) return;

	UWorld* World = PlayerController->GetWorld();
	ADZ_GameState* GameState = World ? World->GetGameState<ADZ_GameState>() : nullptr;
	if (!IsValid(GameState)) return;

	ViewModel->SetElapsedSeconds(GameState->GetElapsedTime());
}
