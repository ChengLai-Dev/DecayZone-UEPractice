// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_InteractController.h"

#include "Blueprint/UserWidget.h"
#include "Data/Input/DZ_InputConfig.h"
#include "Data/UI/DZ_UIConfig.h"
#include "InputMappingContext.h"
#include "Interface/DZ_Interactable.h"
#include "Player/DZ_PlayerController.h"
#include "UI/Widgets/Interaction/DZ_InteractProgress.h"
#include "UI/Widgets/Interaction/DZ_InteractPrompt.h"

void UDZ_InteractController::Initialize(ADZ_PlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig)) return;

	PromptWidget = CreateWidget<UDZ_InteractPrompt>(PlayerController, UIConfig->InteractPromptClass);
	ProgressWidget = CreateWidget<UDZ_InteractProgress>(PlayerController, UIConfig->InteractProgressClass);
}

void UDZ_InteractController::Shutdown()
{
	if (IsValid(PromptWidget))
	{
		PromptWidget->RemoveFromParent();
	}
	if (IsValid(ProgressWidget))
	{
		ProgressWidget->RemoveFromParent();
	}
	PromptWidget = nullptr;
	ProgressWidget = nullptr;
	Tracker.Reset();
	Super::Shutdown();
}

void UDZ_InteractController::Tick(float DeltaSeconds)
{
	if (!Tracker.IsActive()) return;

	Tracker.Tick(DeltaSeconds);
	UpdateProgress();
}

void UDZ_InteractController::Interact(AActor* InteractActor)
{
	if (!PlayerController || !PlayerController->IsLocalController()) return;

	if (Tracker.IsActive())
	{
		if (Tracker.IsAuto()) return;
		CancelInteract();
		return;
	}

	if (!IsValid(InteractActor)) return;

	BeginInteract(InteractActor);
}

void UDZ_InteractController::BeginInteract(AActor* Target)
{
	if (!PlayerController || !PlayerController->IsLocalController()) return;
	if (Tracker.IsActive()) return;

	if (!IsValid(Target)) return;
	if (!Target->Implements<UDZ_Interactable>()) return;
	if (!IDZ_Interactable::Execute_CanInteract(Target)) return;

	const bool bAuto = IDZ_Interactable::Execute_IsAutoInteract(Target);
	const float Duration = IDZ_Interactable::Execute_GetInteractDuration(Target);

	Tracker.Begin(Target, Duration, bAuto);

	if (!bAuto && IsValid(PromptWidget))
	{
		PromptWidget->Hide();
	}
	if (Duration > 0.f && IsValid(ProgressWidget))
	{
		ProgressWidget->Open();
	}

	PlayerController->ServerBeginInteract(Target);
}

void UDZ_InteractController::CancelInteract()
{
	if (!Tracker.IsActive()) return;

	EndInteract();
	if (PlayerController)
	{
		PlayerController->ServerCancelInteract();
	}
}

void UDZ_InteractController::EndInteract()
{
	if (!Tracker.IsActive()) return;

	const bool bAuto = Tracker.IsAuto();
	Tracker.Reset();

	if (IsValid(ProgressWidget))
	{
		ProgressWidget->Close();
	}
	if (!bAuto && IsValid(PromptWidget))
	{
		PromptWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UDZ_InteractController::UpdateProgress()
{
	if (!Tracker.IsActive()) return;
	if (!IsValid(ProgressWidget)) return;

	ProgressWidget->SetProgress(Tracker.GetProgress(), Tracker.GetRemaining());
}

void UDZ_InteractController::OnFocusInteractableChanged(AActor* Actor)
{
	if (!PlayerController || !PlayerController->IsLocalController()) return;
	if (!IsValid(PromptWidget)) return;

	if (!IsValid(Actor))
	{
		PromptWidget->Close();
		return;
	}

	if (!Cast<IDZ_Interactable>(Actor)) return;

	PromptWidget->Open();
	UpdatePromptKeyText();
	PromptWidget->SetPromptText(IDZ_Interactable::Execute_GetInteractPromptText(Actor));
}

void UDZ_InteractController::UpdatePromptKeyText()
{
	const UDZ_InputConfig* InputConfig = PlayerController ? PlayerController->GetInputConfig() : nullptr;
	if (!InputConfig || !IsValid(InputConfig->DefaultInputMappingContext)) return;

	for (const FEnhancedActionKeyMapping& Mapping : InputConfig->DefaultInputMappingContext->GetMappings())
	{
		if (Mapping.Action == InputConfig->InteractAction)
		{
			PromptWidget->SetKeyText(Mapping.Key.GetDisplayName());
			return;
		}
	}
}

bool UDZ_InteractController::IsBlockingAttack() const
{
	return Tracker.IsActive() && !Tracker.IsAuto();
}
