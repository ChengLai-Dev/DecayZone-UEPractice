// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_LootPanel.generated.h"

struct FDZ_ItemStack;
class UDZ_LootListPanel;
class UDZ_BackpackPanel;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_LootPanel : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	
	void SetLootListInfo(const TArray<FDZ_ItemStack>& LootList);
	void SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_LootListPanel> LootListPanel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_BackpackPanel> BackpackPanel;
	
};
