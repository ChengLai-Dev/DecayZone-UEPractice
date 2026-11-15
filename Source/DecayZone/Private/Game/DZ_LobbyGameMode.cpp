// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DZ_LobbyGameMode.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/DZ_PlayerState.h"

ADZ_LobbyGameMode::ADZ_LobbyGameMode()
{
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
	PlayerStateClass = ADZ_PlayerState::StaticClass();
}

void ADZ_LobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (!IsValid(NewPlayer)) return;

	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsWithTag(this, LobbyCameraTag, Cameras);
	if (Cameras.IsEmpty()) return;

	NewPlayer->SetViewTargetWithBlend(Cameras[0], 0.f);
}
