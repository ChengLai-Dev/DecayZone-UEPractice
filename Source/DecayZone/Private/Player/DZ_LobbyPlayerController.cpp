// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_LobbyPlayerController.h"

#include "DecayZone/DecayZone.h"
#include "Game/DZ_GameInstance.h"
#include "UI/Controllers/DZ_StashController.h"
#include "UI/DZ_LobbyUIManager.h"

UUIManager* ADZ_LobbyPlayerController::CreateUIManager()
{
	LobbyUI = NewObject<UDZ_LobbyUIManager>(this);
	return LobbyUI;
}

void ADZ_LobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsValid(LobbyUI)) return;
	if (!IsValid(LobbyUI->StashController)) return;
	
	LobbyUI->StashController->Open();
}

void ADZ_LobbyPlayerController::StartRaid()
{
	UDZ_GameInstance* GameInstance = GetGameInstance<UDZ_GameInstance>();
	if (!IsValid(GameInstance)) return;
	
	if (!GameInstance->TakeDepartureLoadout()) return;
	
	ServerStartRaid(GameInstance->GetPendingLoadout());
}

void ADZ_LobbyPlayerController::ServerStartRaid_Implementation(const FDZ_Loadout& Loadout)
{
	UDZ_GameInstance* GameInstance = GetGameInstance<UDZ_GameInstance>();
	if (!IsValid(GameInstance)) return;
	
	const FName RaidMapName = GameInstance->GetRaidMapName();
	if (RaidMapName.IsNone())
	{
		UE_LOG(LogDZ, Warning, TEXT("RaidMapName is not configured on GameInstance"));
		return;
	}
	
	GameInstance->SetPendingLoadout(Loadout);
	
	GetWorld()->ServerTravel(RaidMapName.ToString());
}
