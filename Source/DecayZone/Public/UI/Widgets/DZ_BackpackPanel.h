// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_ItemSlot.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_BackpackPanel.generated.h"

class UDZ_WeightBar;
/**
 * 
 */
class UDZ_ItemSlot;
class UTextBlock;

UCLASS()
class DECAYZONE_API UDZ_BackpackPanel : public UDZ_UserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
public:
	
	UPROPERTY()
	TArray<TObjectPtr<UDZ_ItemSlot>> ItemSlots;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnItemSlotClicked;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_WeightValue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_WeightBar> WeightBar;
	
private:
	UFUNCTION()
	void HandleItemSlotClicked(UWidget* Widget);
};
