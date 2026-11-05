// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/DZ_Interactable.h"
#include "DZ_InteractableActor.generated.h"

struct FDZ_ItemData;

UCLASS()
class DECAYZONE_API ADZ_InteractableActor : public AActor, public  IDZ_Interactable
{
	GENERATED_BODY()

public:
	ADZ_InteractableActor();
	virtual FText GetInteractPromptText_Implementation() override;
	
	virtual bool CanInteract_Implementation(AActor* Interactor) override;
	virtual float GetInteractDuration_Implementation() override;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Interaction")
	float InteractDuration = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Interaction")
	FText InteractPromptText;
private:
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
};
