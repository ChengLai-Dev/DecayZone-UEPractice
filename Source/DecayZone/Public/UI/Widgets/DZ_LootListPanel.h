// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_ItemSlotBase.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_LootListPanel.generated.h"

class UDZ_LootRow;
struct FDZ_ItemStack;
class UScrollBox;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_LootListPanel : public UDZ_ItemSlotBase
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	void SetContainerName(const FText& Name);

	void SetLootListInfo(const TArray<FDZ_ItemStack>& ItemList);
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent LootItemMoveRequest;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnTakeRequested;
	
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnCloseRequested;
	
protected:
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
private:

	UFUNCTION()
	void HandleLootRowItemDrop(UWidget* From, UWidget* To);
	
	UFUNCTION()
	void HandleLootRowTakeRequested(UWidget* Row);
	
	UFUNCTION()
	void HandleCloseClicked();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_LootCount;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ContainerName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Btn_Close;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_EmptyHint;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_LootList;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDZ_LootRow> LootRowClass;
};
