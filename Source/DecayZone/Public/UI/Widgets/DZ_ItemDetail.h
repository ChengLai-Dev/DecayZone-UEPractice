// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_ItemDetail.generated.h"

class UVerticalBox;
class UWidgetSwitcher;
/**
 * 
 */

enum ItemDetailShowType
{
	Bag,
	Item,
};

UCLASS()
class DECAYZONE_API UDZ_ItemDetail : public UDZ_UserWidget
{
	GENERATED_BODY()
	
	
public:
	
	void SetItemInfo(int32 ItemID);
	
	void SwitchShowType(ItemDetailShowType ShowType);
	

private:
	
	void SetItemState(int32 ItemID);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VB_ItemState;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher_Detail;
};
