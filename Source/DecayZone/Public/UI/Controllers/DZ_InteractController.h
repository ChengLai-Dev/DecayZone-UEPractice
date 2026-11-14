// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "UI/FDZ_InteractTracker.h"
#include "DZ_InteractController.generated.h"

class UDZ_InteractPrompt;
class UDZ_InteractProgress;

UCLASS()
class DECAYZONE_API UDZ_InteractController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(ADZ_PlayerController* InPlayerController) override;
	virtual void Shutdown() override;
	virtual void Tick(float DeltaSeconds) override;

	void Interact(AActor* InteractActor);
	void BeginInteract(AActor* Target);
	void CancelInteract();
	void EndInteract();
	void UpdateProgress();
	void OnFocusInteractableChanged(AActor* Actor);

	bool IsBlockingAttack() const;
	bool IsActive() const { return Tracker.IsActive(); }

private:
	void UpdatePromptKeyText();

	UPROPERTY()
	TObjectPtr<UDZ_InteractPrompt> PromptWidget;

	UPROPERTY()
	TObjectPtr<UDZ_InteractProgress> ProgressWidget;

	FDZ_InteractTracker Tracker;
};
