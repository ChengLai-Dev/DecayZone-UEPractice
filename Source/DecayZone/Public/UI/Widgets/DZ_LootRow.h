// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_LootRow.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_LootRow : public UDZ_UserWidget
{
	GENERATED_BODY()
public:
	void SetItemInfo(int32 ItemID, int32 Quantity);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemWeight;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Quantity;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;
};
