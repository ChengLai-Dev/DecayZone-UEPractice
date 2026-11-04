// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_ItemSlot.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Library/ItemHelper.h"

void UDZ_ItemSlot::SetItemInfo(int32 ItemID)
{
	CurrentItemID = ItemID;
	const FDZ_ItemData* Data = ItemHelper::GetItemData(this, ItemID);
	if (!Data || !Data->Icon)
	{
		Img_ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Img_ItemIcon->SetVisibility(ESlateVisibility::Visible);
	Img_ItemIcon->SetBrushFromTexture(Data->Icon);
}

void UDZ_ItemSlot::SetSelected(bool IsSelected)
{
	const FLinearColor SelectedColor = FLinearColor(FColor::FromHex(TEXT("C9A968")));
	const FLinearColor NormalColor = FLinearColor(FColor::FromHex(TEXT("2E3C44")));
	Border_Selected->SetBrushColor(IsSelected ? SelectedColor : NormalColor);
}

int32 UDZ_ItemSlot::GetItemID()
{
	return CurrentItemID;
}

