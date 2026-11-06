// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_InventoryPanel.generated.h"

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
	UFUNCTION()
	void HandleItemDrop(UWidget* From, UWidget* To);
	virtual void NativeOnInitialized() override;
	
	void SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_ItemDetail> ItemDetail;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_BackpackPanel> BackpackPanel;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnItemMoveRequested;
	
private:
	UFUNCTION()
	void HandleItemSlotClicked(UWidget* Widget);
	
	TWeakObjectPtr<UDZ_ItemSlot> CurrentSelectedItem;
};
