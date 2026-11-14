// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Settlement/DZ_SettlementPanel.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "Data/UI/FDZ_SettlementInfo.h"
#include "UI/Widgets/Settlement/DZ_SettlementItemSlot.h"
#include "UI/Widgets/Settlement/DZ_SettlementStatCard.h"

void UDZ_SettlementPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (Btn_Confirm)
	{
		Btn_Confirm->OnClicked.AddDynamic(this, &ThisClass::HandleConfirmClicked);
	}
}

void UDZ_SettlementPanel::SetSettlementInfo(const FDZ_SettlementInfo& Info)
{
	const FLinearColor SuccessColor = FLinearColor(FColor::FromHex(TEXT("C9A968")));
	const FLinearColor FailColor = FLinearColor(FColor::FromHex(TEXT("8C2B2B")));
	const TCHAR* Title = Info.bSucceeded ? TEXT("撤离成功") : TEXT("撤离失败");
	const TCHAR* SectionTitle = Info.bSucceeded ? TEXT("带出物品") : TEXT("丢失的物品");

	Text_Title->SetText(FText::FromString(Title));
	Text_Title->SetColorAndOpacity(FSlateColor(Info.bSucceeded ? SuccessColor : FailColor));
	Text_Subtitle->SetText(FText::FromString(FString::Printf(TEXT("本次行动用时 %s"), *FormatDuration(Info.ElapsedSeconds).ToString())));

	StatCard_Value->SetStat(Info.TotalValue, FText::FromString(TEXT("总价值")));
	StatCard_EnemyKills->SetStat(Info.EnemyKills, FText::FromString(TEXT("击杀丧尸")));
	StatCard_PlayerKills->SetStat(Info.PlayerKills, FText::FromString(TEXT("击杀玩家")));

	if (Text_SectionTitle)
	{
		Text_SectionTitle->SetText(FText::FromString(SectionTitle));
	}

	RefreshItems(Info.Items);
}

void UDZ_SettlementPanel::RefreshItems(const TArray<FDZ_ItemStack>& Items)
{
	if (!WrapBox_Items || !ScrollBox_Items || !Text_LootCount || !Text_EmptyHint) return;

	WrapBox_Items->ClearChildren();

	int32 TotalCount = 0;
	for (const FDZ_ItemStack& Item : Items)
	{
		TotalCount += Item.Count;
	}
	Text_LootCount->SetText(FText::FromString(FString::Printf(TEXT("共 %d 件"), TotalCount)));

	if (Items.IsEmpty())
	{
		ScrollBox_Items->SetVisibility(ESlateVisibility::Collapsed);
		Text_EmptyHint->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	ScrollBox_Items->SetVisibility(ESlateVisibility::Visible);
	Text_EmptyHint->SetVisibility(ESlateVisibility::Collapsed);

	if (!ItemSlotClass) return;

	for (const FDZ_ItemStack& Item : Items)
	{
		UDZ_SettlementItemSlot* ItemSlot = CreateWidget<UDZ_SettlementItemSlot>(this, ItemSlotClass);
		if (!ItemSlot) continue;

		ItemSlot->SetItem(Item.ItemId, Item.Count);
		WrapBox_Items->AddChildToWrapBox(ItemSlot);
	}
}

void UDZ_SettlementPanel::HandleConfirmClicked()
{
	OnConfirmRequested.Broadcast();
}

FText UDZ_SettlementPanel::FormatDuration(float Seconds)
{
	const int32 TotalSeconds = FMath::Max(0, FMath::FloorToInt(Seconds));
	const int32 Minutes = TotalSeconds / 60;
	const int32 RemainingSeconds = TotalSeconds % 60;
	return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, RemainingSeconds));
}
