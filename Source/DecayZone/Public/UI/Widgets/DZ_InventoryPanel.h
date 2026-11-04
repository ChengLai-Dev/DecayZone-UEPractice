// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_InventoryPanel.generated.h"

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
	virtual void NativeConstruct() override;
	


	UPROPERTY()
	TArray<TObjectPtr<UDZ_ItemSlot>> ItemSlots;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_Backpack;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_ItemDetail> ItemDetail;
	
private:
	UFUNCTION()
	void OnItemSlotClicked(UWidget* Widget);
	
	TWeakObjectPtr<UDZ_ItemSlot> CurrentSelectedItem;
};
