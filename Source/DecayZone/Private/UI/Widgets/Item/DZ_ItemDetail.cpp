// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Item/DZ_ItemDetail.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Library/ItemHelper.h"
#include "Library/UIHelper.h"

void UDZ_ItemDetail::SetItemInfo(int32 ItemID)
{
	SetItemState(ItemID);
}

void UDZ_ItemDetail::SwitchShowType(ItemDetailShowType ShowType)
{
	Switcher_Detail->SetActiveWidgetIndex(ShowType);
}

void UDZ_ItemDetail::SetItemState(int32 ItemID)
{
	if (ItemID == DZItem::EmptyId) return;
	const FDZ_ItemData* Data = ItemHelper::GetItemData(this, ItemID);
	
	UTextBlock* Text_ItemName = Cast<UTextBlock>(UIHelper::GetChildByName(VB_ItemState, "Text_ItemName"));
	UTextBlock* Text_ItemQuantity = Cast<UTextBlock>(UIHelper::GetChildByName(VB_ItemState, "Text_ItemQuantity"));
	UTextBlock* Text_ItemType = Cast<UTextBlock>(UIHelper::SeekWidget(VB_ItemState, "HB_Meta.Text_ItemType"));
	UTextBlock* Text_ItemWeight = Cast<UTextBlock>(UIHelper::SeekWidget(VB_ItemState, "HB_Meta.Text_ItemWeight"));
	UImage* Img_ItemIcon = Cast<UImage>(UIHelper::SeekWidget(VB_ItemState, "Size_IconFrame.Border_IconFrame.Fill_IconFrame.Overlay_Icon.Size_ItemIcon.Img_ItemIcon"));
	
	Text_ItemName->SetText(Data->ItemName);
	Img_ItemIcon->SetBrushFromTexture(Data->Icon);
}
