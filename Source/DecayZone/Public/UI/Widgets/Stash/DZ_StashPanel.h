// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_StashPanel.generated.h"

class UButton;
class UTextBlock;
class UUniformGridPanel;
class UDZ_EquipmentSlot;
class UDZ_ItemSlot;
struct FDZ_ItemStack;

UCLASS()
class DECAYZONE_API UDZ_StashPanel : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void SetStashItems(const TArray<FDZ_ItemStack>& Items);
	void SetStashSummary(int32 Used, int32 Capacity);
	void SetStashTotalValue(int32 Value);
	void SetBagItems(const TArray<FDZ_ItemStack>& Items);
	void SetBagSummary(int32 Used, int32 Capacity);
	void SetEquipmentInfo(const TArray<int32>& Equipments);

	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnItemMoveRequested;

	UPROPERTY(BlueprintAssignable)
	FOnEvent OnStartRequested;

private:
	UFUNCTION()
	void HandleItemDrop(UWidget* From, UWidget* To);

	UFUNCTION()
	void HandleStartClicked();

	void CollectSlots(UUniformGridPanel* Grid, EItemContainer Container, TArray<TObjectPtr<UDZ_ItemSlot>>& OutSlots);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_Stash;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StashCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_TotalValue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> Grid_LoadoutBag;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_BagCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Armor;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Primary;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Melee;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Backpack;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Start;

	UPROPERTY()
	TArray<TObjectPtr<UDZ_ItemSlot>> StashSlots;

	UPROPERTY()
	TArray<TObjectPtr<UDZ_ItemSlot>> BagSlots;
};
