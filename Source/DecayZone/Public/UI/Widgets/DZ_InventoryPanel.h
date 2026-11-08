// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_InventoryPanel.generated.h"

class UDZ_EquipmentPanel;
struct FDZ_ItemStack;
class UDZ_BackpackPanel;
class UDZ_ItemDetail;
class UUniformGridPanel;
class UDZ_ItemSlot;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_InventoryPanel : public UDZ_UserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeOnInitialized() override;
	
	void SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo);
	void SetEquipmentPanel(const TArray<int32>& EquipmentInfo);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_ItemDetail> ItemDetail;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_BackpackPanel> BackpackPanel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentPanel> EquipmentPanel;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnItemMoveRequested;
	
private:
	UFUNCTION()
	void HandleItemDrop(UWidget* From, UWidget* To);
	
	UFUNCTION()
	void HandleItemSlotClicked(UWidget* Widget);
	void RefreshItemDetail();

	TWeakObjectPtr<UDZ_ItemSlot> CurrentSelectedItem;
};
