// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Item/DZ_ItemSlot.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Library/ItemHelper.h"

void UDZ_ItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDZ_ItemSlot::SetItemInfo(int32 ItemID, int32 Quantity)
{
	CurrentItemID = ItemID;
	if (ItemID == DZItem::EmptyId || Quantity <= 0)
	{
		SetEmptyState(true);
		return;
	}

	const FDZ_ItemData* Data = ItemHelper::GetItemData(this, ItemID);
	Img_ItemIcon->SetVisibility(ESlateVisibility::Visible);
	Img_ItemIcon->SetBrushFromTexture(Data->Icon);
	Text_Quantity->SetText(FText::AsNumber(Quantity));

	SetEmptyState(false);
	if (Quantity == 1)
	{
		Text_Quantity->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		Text_Quantity->SetVisibility(ESlateVisibility::Visible);
	}
}

void UDZ_ItemSlot::SetSelected(bool IsSelected)
{
	const FLinearColor SelectedColor = FLinearColor(FColor::FromHex(TEXT("C9A968")));
	const FLinearColor NormalColor = FLinearColor(FColor::FromHex(TEXT("2E3C44")));
	Border_Selected->SetBrushColor(IsSelected ? SelectedColor : NormalColor);
}

void UDZ_ItemSlot::SetEmptyState(bool IsEmpty)
{
	if (IsEmpty)
	{
		SetDragEnabled(false);
		Overlay_Slot->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SetDragEnabled(true);
		Overlay_Slot->SetVisibility(ESlateVisibility::Visible);
	}
	bIsEmptyState = IsEmpty;
}

bool UDZ_ItemSlot::IsEmptyState()
{
	return bIsEmptyState;
}

UDZ_UserWidget* UDZ_ItemSlot::CreateDragVisualWidget()
{
	UDZ_ItemSlot* VisualWidget = CreateWidget<UDZ_ItemSlot>(GetOwningPlayer(), GetClass());
	if (VisualWidget)
	{
		VisualWidget->SetItemInfo(CurrentItemID, 1);
	}
	return VisualWidget;
}

int32 UDZ_ItemSlot::GetItemID()
{
	return CurrentItemID;
}

