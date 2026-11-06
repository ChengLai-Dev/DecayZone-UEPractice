// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DZ_InputConfig.generated.h"

class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_InputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UInputAction> InteractAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UInputAction> OpenOrCloseInventoryAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|InputConfig")
	TObjectPtr<UInputAction> CloseTopWidgetAction;
	
};
