// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_SettlementItemSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Library/ItemHelper.h"

void UDZ_SettlementItemSlot::SetItem(int32 ItemId, int32 Count)
{
	if (ItemId == DZItem::EmptyId || Count <= 0)
	{
		Img_ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		Text_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const FDZ_ItemData* Data = ItemHelper::GetItemData(this, ItemId);
	if (Data && Data->Icon)
	{
		Img_ItemIcon->SetBrushFromTexture(Data->Icon);
		Img_ItemIcon->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Img_ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	}

	Text_Quantity->SetText(FText::FromString(FString::Printf(TEXT("x%d"), Count)));
	Text_Quantity->SetVisibility(Count > 1 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}
