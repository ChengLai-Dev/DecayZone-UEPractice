// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DZ_Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UDZ_Interactable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DECAYZONE_API IDZ_Interactable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanInteract() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetInteractDuration() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FText GetInteractPromptText() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsAutoInteract() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnInteractStarted(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnInteractCompleted(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnInteractCancelled(AActor* Interactor);
};
