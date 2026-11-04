// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_ItemSlot.generated.h"

class UBorder;
class UImage;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotClicked, UDZ_ItemSlot*, ClickedSlot);

UCLASS()
class DECAYZONE_API UDZ_ItemSlot : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemInfo(int32 ItemID);
	void SetSelected(bool IsSelected);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Selected;
	
	int32 GetItemID();
private:
	int32 CurrentItemID = 0;
};
