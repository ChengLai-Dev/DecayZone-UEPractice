// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Loot/DZ_LootRow.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Library/ItemHelper.h"
#include "UI/Widgets/Item/DZ_ItemSlot.h"

UDZ_LootRow::UDZ_LootRow()
{
	SetDragEnabled(true);
}

void UDZ_LootRow::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (Btn_Take)
	{
		Btn_Take->OnClicked.AddDynamic(this, &ThisClass::HandleTakeClicked);
	}
}

void UDZ_LootRow::HandleTakeClicked()
{
	OnTakeRequested.Broadcast(this);
}

bool UDZ_LootRow::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return false;
}

void UDZ_LootRow::SetItemInfo(int32 ItemID, int32 Quantity)
{
	if (ItemID == DZItem::EmptyId) return;
	const FDZ_ItemData* Data = ItemHelper::GetItemData(this, ItemID);
	Text_ItemWeight->SetText(FText::FromString(FString::Printf(TEXT("%.1f KG"), Data->Weight)));
	Text_ItemName->SetText(Data->ItemName);
	Text_Quantity->SetText(FText::AsNumber(Quantity));
	Img_ItemIcon->SetBrushFromTexture(Data->Icon);
	Img_ItemIcon->SetVisibility(ESlateVisibility::Visible);
	CurrentItemID = ItemID;
}

UDZ_UserWidget* UDZ_LootRow::CreateDragVisualWidget()
{
	UDZ_ItemSlot* VisualWidget = CreateWidget<UDZ_ItemSlot>(GetOwningPlayer(), DragVisualClass);
	VisualWidget->SetItemInfo(CurrentItemID, 1);
	return VisualWidget;
}
