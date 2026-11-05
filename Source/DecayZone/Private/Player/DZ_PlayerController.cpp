// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Character/DZ_PlayerCharacter.h"
#include "Components/DZ_ContainerComponent.h"
#include "Data/Input/DZ_InputConfig.h"
#include "Data/UI/DZ_UIConfig.h"
#include "DecayZone/DecayZone.h"
#include "GameFramework/Character.h"
#include "Interface/DZ_Interactable.h"
#include "UI/Widgets/DZ_InteractProgress.h"
#include "UI/Widgets/DZ_InventoryPanel.h"
#include "UI/Widgets/DZ_InteractPrompt.h"


void ADZ_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!IsValid(InputConfig))
	{
		UE_LOG(LogDZ, Warning, TEXT("no InputConfig found"));
		return;
	}
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	Subsystem->AddMappingContext(InputConfig->DefaultInputMappingContext, 0);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ADZ_PlayerController::Look);
	EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ADZ_PlayerController::Jump);
	EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ADZ_PlayerController::Move);
	EnhancedInputComponent->BindAction(InputConfig->OpenOrCloseInventoryAction, ETriggerEvent::Started, this, &ADZ_PlayerController::OpenOrCloseInventoryPanel);
	EnhancedInputComponent->BindAction(InputConfig->InteractAction, ETriggerEvent::Started, this, &ADZ_PlayerController::Interact);
	
}

void ADZ_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	InventoryWidget = CreateWidget<UDZ_InventoryPanel>(this, UIConfig->InventoryWidgetClass);
	InteractPromptWidget = CreateWidget<UDZ_InteractPrompt>(this, UIConfig->InteractPromptClass);
	InteractProgressWidget = CreateWidget<UDZ_InteractProgress>(this, UIConfig->InteractProgressClass);
}
void ADZ_PlayerController::Interact()
{
	InteractProgressWidget->
}

void ADZ_PlayerController::HandleInteractableActorChanged(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		InteractPromptWidget->RemoveFromParent();
		return;
	}
	
	if (IDZ_Interactable* InteractableActor = Cast<IDZ_Interactable>(Actor))
	{
		InteractPromptWidget->AddToViewport();
		FText InteractPrompt = IDZ_Interactable::Execute_GetInteractPromptText(Actor);
		FText InteractKey; 
		for (const auto& Mapping: InputConfig->DefaultInputMappingContext->GetMappings())
		{
			if (Mapping.Action == InputConfig->InteractAction)
			{
				InteractKey = Mapping.Key.GetDisplayName();
				break;
			}
		}
		InteractPromptWidget->SetKeyText(InteractKey);
		InteractPromptWidget->SetPromptText(InteractPrompt);
	}
}

void ADZ_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(InPawn))
	{
		PlayerCharacter->OnFocusInteractableActorChanged.AddDynamic(this, &ADZ_PlayerController::HandleInteractableActorChanged);
	}
}

void ADZ_PlayerController::ServerTakeItem_Implementation(AActor* ContainerActor, int32 SlotIndex, int32 Count)
{
	if (!IsValid(ContainerActor)) return;

	UDZ_ContainerComponent* Container = ContainerActor->FindComponentByClass<UDZ_ContainerComponent>();
	if (!Container) return;

	Container->TryTake(SlotIndex, Count);
}

void ADZ_PlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	const FRotator YawRotation = FRotator(0, GetControlRotation().Yaw, 0);
	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	GetPawn()->AddMovementInput(ForwardVector, InputVector.Y);
	GetPawn()->AddMovementInput(RightVector, InputVector.X);
}

void ADZ_PlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	AddYawInput(InputVector.X);
	AddPitchInput(InputVector.Y);
}

void ADZ_PlayerController::Jump(const FInputActionValue& Value)
{
	GetCharacter()->Jump();
}

void ADZ_PlayerController::OpenOrCloseInventoryPanel()
{
	if (!InventoryWidget) return;
	
	if (InventoryWidget->IsInViewport())
	{
		InventoryWidget->RemoveFromParent();
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
	else
	{
		InventoryWidget->AddToViewport();
		SetInputMode(FInputModeGameAndUI());
		bShowMouseCursor = true;
	}
}
