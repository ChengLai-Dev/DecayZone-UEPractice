// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DZ_UIConfig.generated.h"

class UDZ_UserWidget;
class UDZ_InventoryPanel;
/**
 * 
 */

UCLASS()
class DECAYZONE_API UDZ_UIConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TSubclassOf<UDZ_InventoryPanel> InventoryWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TMap<TSubclassOf<UDZ_UserWidget>, int32> ZOrderMap;
};
