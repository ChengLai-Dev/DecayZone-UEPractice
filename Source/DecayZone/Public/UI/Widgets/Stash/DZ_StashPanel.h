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

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStashSlotEvent, FDZ_ItemSlotHandle, Handle);

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

	void SetSellInfo(const FText& Info, int32 Price);
	void ClearSellInfo();

	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnItemMoveRequested;

	UPROPERTY(BlueprintAssignable)
	FOnEvent OnStartRequested;

	UPROPERTY(BlueprintAssignable)
	FOnEvent OnShopRequested;

	/** 出售目标变化：选中格变了，或选中格里的物品数量变了。Handle 无效表示没有目标。 */
	UPROPERTY(BlueprintAssignable)
	FOnStashSlotEvent OnSellTargetChanged;

	/** 点了「出售」，参数就是当前出售目标的地址。 */
	UPROPERTY(BlueprintAssignable)
	FOnStashSlotEvent OnSellRequested;

private:
	UFUNCTION()
	void HandleItemDrop(UWidget* From, UWidget* To);

	UFUNCTION()
	void HandleStartClicked();

	UFUNCTION()
	void HandleShopClicked();

	UFUNCTION()
	void HandleStashSlotClicked(UWidget* Widget);

	UFUNCTION()
	void HandleSellClicked();

	void CollectSlots(UUniformGridPanel* Grid, EItemContainer Container, TArray<TObjectPtr<UDZ_ItemSlot>>& OutSlots);
	void SetStashSelection(const FDZ_ItemSlotHandle& Handle);
	void ClearStashSelection();
	void RefreshSlotSelection();

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Shop;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SellInfo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SellPrice;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Sell;

	UPROPERTY()
	TArray<TObjectPtr<UDZ_ItemSlot>> StashSlots;

	UPROPERTY()
	TArray<TObjectPtr<UDZ_ItemSlot>> BagSlots;

	FDZ_ItemSlotHandle SelectedSlot;
};
