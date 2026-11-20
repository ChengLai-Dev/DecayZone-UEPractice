// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/List/DZ_ListEntryWidget.h"
#include "DZ_ShopEntryWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;

UCLASS()
class DECAYZONE_API UDZ_ShopEntryWidget : public UDZ_ListEntryWidget
{
	GENERATED_BODY()

public:
	virtual void OnEntryRefresh(UObject* Item) override;

protected:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void HandleBuyClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Name;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Type;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Price;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Buy;
};
