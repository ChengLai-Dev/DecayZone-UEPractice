// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DZ_LobbyGameMode.generated.h"

UCLASS()
class DECAYZONE_API ADZ_LobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADZ_LobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Lobby")
	FName LobbyCameraTag = TEXT("LobbyCamera");
};
