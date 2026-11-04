// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DZ_InteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DECAYZONE_API UDZ_InteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDZ_InteractionComponent();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction")
	float InteractDuration = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction")
	float InteractRange = 200.0f;
};
