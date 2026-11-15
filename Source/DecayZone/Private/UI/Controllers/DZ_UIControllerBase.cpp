// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_UIControllerBase.h"
#include "Game/DZ_GameInstance.h"

void UDZ_UIControllerBase::Initialize(APlayerController* InPlayerController)
{
	PlayerController = InPlayerController;
	if (!PlayerController) return;

	GameInstance = Cast<UDZ_GameInstance>(PlayerController->GetGameInstance());
	if (!IsValid(GameInstance)) return;

	UIConfig = GameInstance->UIConfig;
}

void UDZ_UIControllerBase::Shutdown()
{
	GameInstance = nullptr;
	UIConfig = nullptr;
	PlayerController = nullptr;
}

void UDZ_UIControllerBase::Tick(float DeltaSeconds)
{
}
