// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_PlayerController.h"

#include "Library/UIHelper.h"
#include "UI/UIManager.h"

UUIManager* ADZ_PlayerController::CreateUIManager()
{
	return nullptr;
}

void ADZ_PlayerController::ShowTips(const FText& TipText, float Duration)
{
	ClientShowTips(TipText, Duration);
}

void ADZ_PlayerController::ClientShowTips_Implementation(const FText& TipText, float Duration)
{
	UIHelper::ShowTips(this, TipText, Duration);
}

void ADZ_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalController()) return;
	
	UIManager = CreateUIManager();
	if (!IsValid(UIManager)) return;
	
	UIManager->Initialize(this);
}

void ADZ_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(UIManager))
	{
		UIManager->Shutdown();
	}
	UIManager = nullptr;
	
	Super::EndPlay(EndPlayReason);
}

void ADZ_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!IsLocalController()) return;
	if (!IsValid(UIManager)) return;
	
	UIManager->TickControllers(DeltaSeconds);
}
