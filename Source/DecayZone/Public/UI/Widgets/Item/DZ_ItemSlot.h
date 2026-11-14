// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/Item/DZ_ItemSlotBase.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_ItemSlot.generated.h"

class UOverlay;
class UTextBlock;
class UBorder;
class UImage;
/**
 * 
 */


UCLASS()
class DECAYZONE_API UDZ_ItemSlot : public UDZ_ItemSlotBase
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void SetItemInfo(int32 ItemID, int32 Quantity);
	void SetSelected(bool IsSelected);
	void SetEmptyState(bool IsEmpty);
	bool IsEmptyState();
	virtual UDZ_UserWidget* CreateDragVisualWidget() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Selected;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Quantity;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Slot;
	
	int32 GetItemID();

private:
	bool bIsEmptyState = true;
	int32 CurrentItemID = DZItem::EmptyId;
};
