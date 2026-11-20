// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_ShopController.generated.h"

class UDZ_ShopItemObject;
class UDZ_ShopPanel;

UCLASS()
class DECAYZONE_API UDZ_ShopController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(APlayerController* InPlayerController) override;
	virtual void Shutdown() override;

	void Open();
	void Close();
	bool Toggle();
	void Refresh();

private:
	UFUNCTION()
	void HandleBuyRequested(int32 ItemId);

	UFUNCTION()
	void HandleMoneyChanged();

	UFUNCTION()
	void HandleStashChanged();

	void BuildEntries();
	void ShowTips(const FText& TipText);

	UPROPERTY()
	TObjectPtr<UDZ_ShopPanel> ShopPanelWidget;

	UPROPERTY()
	TArray<TObjectPtr<UDZ_ShopItemObject>> Entries;
};
