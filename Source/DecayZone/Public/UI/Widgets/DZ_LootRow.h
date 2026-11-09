// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_ItemSlotBase.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "DZ_LootRow.generated.h"

class UDZ_ItemSlot;
class UButton;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_LootRow : public UDZ_ItemSlotBase
{
	GENERATED_BODY()
public:
	
	UDZ_LootRow();
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	void SetItemInfo(int32 ItemID, int32 Quantity);
	
	virtual UDZ_UserWidget* CreateDragVisualWidget() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnTakeRequested;
	
private:
	UFUNCTION()
	void HandleTakeClicked();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_Take;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemWeight;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Quantity;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDZ_ItemSlot> DragVisualClass;
	
	int32 CurrentItemID = DZItem::EmptyId;
};
