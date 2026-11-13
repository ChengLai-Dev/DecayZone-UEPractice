// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DZ_GameState.h"

#include "Net/UnrealNetwork.h"

ADZ_GameState::ADZ_GameState()
{
}

void ADZ_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADZ_GameState, StartTime);
}

void ADZ_GameState::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		StartTime = GetServerWorldTimeSeconds();
	}
}

float ADZ_GameState::GetElapsedTime() const
{
	return GetServerWorldTimeSeconds() - StartTime;
}

float ADZ_GameState::GetRunStartTime() const
{
	return StartTime;
}


