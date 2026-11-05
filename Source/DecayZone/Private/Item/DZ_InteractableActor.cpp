// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DZ_InteractableActor.h"

#include "DecayZone/DecayZone.h"

ADZ_InteractableActor::ADZ_InteractableActor()
{
	SetReplicates(true);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionObjectType(Interactable);
}

FText ADZ_InteractableActor::GetInteractPromptText_Implementation()
{
	return InteractPromptText;
}

bool ADZ_InteractableActor::CanInteract_Implementation(AActor* Interactor)
{
	return true;
}

float ADZ_InteractableActor::GetInteractDuration_Implementation()
{
	return InteractDuration;
}
