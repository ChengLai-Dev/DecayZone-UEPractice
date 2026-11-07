// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_EquipmentPanel.generated.h"

enum class EEquipSlot : uint8;
class UDZ_EquipmentSlot;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_EquipmentPanel : public UDZ_UserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeOnInitialized() override;
	
	void SetItemInfo(int32 ItemId, EEquipSlot EquipSlot);
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnItemDrop;
	
private:
	
	UFUNCTION()
	void HandleEquipSlotDrop(UWidget* From, UWidget* To);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Armor;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Backpack;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Melee;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Primary;
	
	TMap<EEquipSlot, TWeakObjectPtr<UDZ_EquipmentSlot>> EquipSlotMap = {
		{ EEquipSlot::Armor, EquipSlot_Armor },
		{ EEquipSlot::Backpack, EquipSlot_Backpack },
		{ EEquipSlot::Melee, EquipSlot_Melee },
		{ EEquipSlot::Primary, EquipSlot_Primary }
	};
};
