// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_UIConfig.generated.h"

class UDZ_Tips;
class UDZ_InteractProgress;
class UDZ_InteractPrompt;
class UDZ_InventoryPanel;
class UDZ_LootPanel;
class UDZ_StatusBars;
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
	TSubclassOf<UDZ_InteractPrompt> InteractPromptClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TSubclassOf<UDZ_InteractProgress> InteractProgressClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TSubclassOf<UDZ_LootPanel> LootPanelClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TSubclassOf<UDZ_StatusBars> StatusBarsClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TSubclassOf<UDZ_Tips> TipsClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TMap<TSubclassOf<UDZ_UserWidget>, int32> ZOrderMap;
	
};
