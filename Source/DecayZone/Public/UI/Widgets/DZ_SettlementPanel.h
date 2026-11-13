// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_SettlementPanel.generated.h"

class UButton;
class UScrollBox;
class UTextBlock;
class UWrapBox;
class UDZ_SettlementStatCard;
class UDZ_SettlementItemSlot;
struct FDZ_ItemStack;
struct FDZ_SettlementInfo;

UCLASS()
class DECAYZONE_API UDZ_SettlementPanel : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void SetSettlementInfo(const FDZ_SettlementInfo& Info);

	UPROPERTY(BlueprintAssignable)
	FOnEvent OnConfirmRequested;

private:
	UFUNCTION()
	void HandleConfirmClicked();

	void RefreshItems(const TArray<FDZ_ItemStack>& Items);

	static FText FormatDuration(float Seconds);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Subtitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_SettlementStatCard> StatCard_Value;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_SettlementStatCard> StatCard_EnemyKills;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_SettlementStatCard> StatCard_PlayerKills;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SectionTitle;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_LootCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Items;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> WrapBox_Items;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_EmptyHint;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Confirm;

	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Settlement")
	TSubclassOf<UDZ_SettlementItemSlot> ItemSlotClass;
};
