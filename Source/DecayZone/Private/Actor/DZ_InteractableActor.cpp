// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DZ_InteractableActor.h"

#include "Components/DZ_ContainerComponent.h"
#include "DecayZone/DecayZone.h"
#include "GameFramework/Pawn.h"
#include "Player/DZ_PlayerController.h"

ADZ_InteractableActor::ADZ_InteractableActor()
{
	SetReplicates(true);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionObjectType(ECC_Interactable);
}

FText ADZ_InteractableActor::GetInteractPromptText_Implementation() const
{
	return InteractPromptText;
}

bool ADZ_InteractableActor::CanInteract_Implementation() const
{
	return true;
}

float ADZ_InteractableActor::GetInteractDuration_Implementation() const
{
	return InteractDuration;
}

bool ADZ_InteractableActor::IsAutoInteract_Implementation() const
{
	return false;
}

void ADZ_InteractableActor::OnInteractCompleted_Implementation(AActor* InteractInstigator, bool bSucceeded)
{
	if (!bSucceeded) return;

	APawn* Pawn = Cast<APawn>(InteractInstigator);
	if (!Pawn) return;

	ADZ_PlayerController* PlayerController = Cast<ADZ_PlayerController>(Pawn->GetController());
	if (!PlayerController) return;
	
	if (UDZ_ContainerComponent* Container = FindComponentByClass<UDZ_ContainerComponent>())
	{
		PlayerController->ClientOpenLootPanel(Container);
	}
}

void ADZ_InteractableActor::NotifyAutoInteractStart(AActor* InteractInstigator)
{
	APawn* Pawn = Cast<APawn>(InteractInstigator);
	if (!Pawn) return;

	ADZ_PlayerController* PlayerController = Cast<ADZ_PlayerController>(Pawn->GetController());
	if (!PlayerController) return;
	
	PlayerController->BeginInteract(this);
}

void ADZ_InteractableActor::NotifyAutoInteractCancel(AActor* InteractInstigator)
{
	APawn* Pawn = Cast<APawn>(InteractInstigator);
	if (!Pawn) return;

	ADZ_PlayerController* PlayerController = Cast<ADZ_PlayerController>(Pawn->GetController());
	if (!PlayerController) return;
	
	PlayerController->CancelInteract();
}
