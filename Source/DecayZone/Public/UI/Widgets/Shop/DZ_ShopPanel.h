// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_ShopPanel.generated.h"

class UButton;
class UDZ_ListView;
class UTextBlock;
class UDZ_ShopItemObject;

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShopBuyRequest, int32, ItemId);

UCLASS()
class DECAYZONE_API UDZ_ShopPanel : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void SetMoney(int32 Money);
	void SetShopItems(const TArray<TObjectPtr<UDZ_ShopItemObject>>& InItems);
	void RefreshEntries();
	void RequestBuy(UDZ_ShopItemObject* ShopItem);

	UPROPERTY(BlueprintAssignable)
	FOnShopBuyRequest OnBuyRequested;

protected:
	UFUNCTION()
	void HandleCloseClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_ListView> List_Items;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Money;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close;

	UPROPERTY()
	TArray<TObjectPtr<UDZ_ShopItemObject>> Items;
};
