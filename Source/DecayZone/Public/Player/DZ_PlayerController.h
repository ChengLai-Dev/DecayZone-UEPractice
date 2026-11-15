// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DZ_PlayerController.generated.h"

class UUIManager;

UCLASS()
class DECAYZONE_API ADZ_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UUIManager* GetUIManager() const { return UIManager; }

	UFUNCTION(BlueprintCallable, Category = "DecayZone|UI")
	void ShowTips(const FText& TipText, float Duration = 3.f);

protected:
	virtual UUIManager* CreateUIManager();

	UPROPERTY()
	TObjectPtr<UUIManager> UIManager;

private:
	UFUNCTION(Client, Reliable)
	void ClientShowTips(const FText& TipText, float Duration);
};
