// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "UI/FDZ_InteractTracker.h"
#include "DZ_InteractController.generated.h"

class ADZ_RaidPlayerController;
class UDZ_InteractPrompt;
class UDZ_InteractProgress;

UCLASS()
class DECAYZONE_API UDZ_InteractController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(APlayerController* InPlayerController) override;
	virtual void Shutdown() override;
	virtual void Tick(float DeltaSeconds) override;

	void Interact(AActor* InteractActor);
	void BeginInteract(AActor* Target);
	void CancelInteract();
	void EndInteract();
	void UpdateProgress();
	void OnFocusInteractableChanged(AActor* Actor);

	bool IsBlockingAction() const;
	bool IsActive() const { return Tracker.IsActive(); }

private:
	void UpdatePromptKeyText();

	UPROPERTY()
	TObjectPtr<UDZ_InteractPrompt> PromptWidget;

	UPROPERTY()
	TObjectPtr<UDZ_InteractProgress> ProgressWidget;

	UPROPERTY()
	TObjectPtr<ADZ_RaidPlayerController> RaidPlayerController;

	FDZ_InteractTracker Tracker;
};
