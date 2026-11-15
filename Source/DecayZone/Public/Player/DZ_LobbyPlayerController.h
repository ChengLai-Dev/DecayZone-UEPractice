// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/DZ_PlayerController.h"
#include "DZ_LobbyPlayerController.generated.h"

class UDZ_LobbyUIManager;

UCLASS()
class DECAYZONE_API ADZ_LobbyPlayerController : public ADZ_PlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void StartRaid();

protected:
	virtual UUIManager* CreateUIManager() override;

private:
	UPROPERTY()
	TObjectPtr<UDZ_LobbyUIManager> LobbyUI;
};
