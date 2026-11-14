// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "UI/Widgets/Inventory/DZ_EquipmentPanel.h"
#include "DZ_LootPanel.generated.h"

struct FDZ_ItemStack;
class UDZ_LootListPanel;
class UDZ_BackpackPanel;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_LootPanel : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeOnInitialized() override;
	void SetLootListInfo(const TArray<FDZ_ItemStack>& LootList);
	void SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo);
	void SetEquipmentPanel(const TArray<int32>& EquipmentInfo);
	void SetCharacterStatsInfo(const FDZ_CharacterStatsInfo& StatsInfo);
	void SetWeightInfo(float Current, float Max);
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnItemMoveRequested;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnTakeItemRequested;
	
private:
	UFUNCTION()
	void HandleItemDrop(UWidget* From, UWidget* To);
	
	UFUNCTION()
	void HandleTakeRequested(UWidget* Row);
	
	UFUNCTION()
	void HandleCloseRequested();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_LootListPanel> LootListPanel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_BackpackPanel> BackpackPanel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentPanel> EquipmentPanel;
	
};
