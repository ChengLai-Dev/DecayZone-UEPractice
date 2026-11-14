// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_SettlementItemSlot.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class DECAYZONE_API UDZ_SettlementItemSlot : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	void SetItem(int32 ItemId, int32 Count);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_ItemIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Quantity;
};
