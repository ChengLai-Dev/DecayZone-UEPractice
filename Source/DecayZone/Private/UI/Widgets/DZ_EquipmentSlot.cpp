// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_EquipmentSlot.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Library/ItemHelper.h"

void UDZ_EquipmentSlot::SetItemInfo(int32 ItemId)
{
	const FDZ_ItemData* Data = ItemHelper::GetItemData(this, ItemId);
	Img_EquipIcon->SetVisibility(ESlateVisibility::Visible);
	Img_EquipIcon->SetBrushFromTexture(Data->Icon);
	Text_EquipName->SetText(Data->ItemName);
}

void UDZ_EquipmentSlot::SetEmpty()
{
	Img_EquipIcon->SetVisibility(ESlateVisibility::Hidden);
	Text_EquipName->SetText(FText::FromString(TEXT("未装备")));
}
