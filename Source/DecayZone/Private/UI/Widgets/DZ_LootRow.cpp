// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_LootRow.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Library/ItemHelper.h"

void UDZ_LootRow::SetItemInfo(int32 ItemID, int32 Quantity)
{
	const FDZ_ItemData* Data = ItemHelper::GetItemData(this, ItemID);
	Text_ItemWeight->SetText(FText::FromString(FString::Printf(TEXT("%.1f KG"), Data->Weight)));
	Text_ItemName->SetText(Data->ItemName);
	Text_Quantity->SetText(FText::AsNumber(Quantity));
	Img_ItemIcon->SetBrushFromTexture(Data->Icon);
	Img_ItemIcon->SetVisibility(ESlateVisibility::Visible);
}
