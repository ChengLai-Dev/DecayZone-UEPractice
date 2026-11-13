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
	virtual FText GetInteractPromptText_Implementation() const override;
	
	virtual bool CanInteract_Implementation() const override;
	virtual float GetInteractDuration_Implementation() const override;
	virtual bool IsAutoInteract_Implementation() const override;
	virtual void OnInteractCompleted_Implementation(AActor* Interactor, bool bSucceeded) override;
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Interaction")
	float InteractDuration = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Interaction")
	FText InteractPromptText;
	
	UFUNCTION(BlueprintCallable)
	void NotifyAutoInteractStart(AActor* Interactor);

	UFUNCTION(BlueprintCallable)
	void NotifyAutoInteractCancel(AActor* Interactor);
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
};
