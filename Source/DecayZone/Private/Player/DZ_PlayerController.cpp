// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Data/Input/DZ_InputConfig.h"
#include "DecayZone/DecayZone.h"
#include "GameFramework/Character.h"

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

