// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Loot/DZ_LootListPanel.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "UI/Widgets/Loot/DZ_LootRow.h"

void UDZ_LootListPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ScrollBox_LootList->ClearChildren();
	SetSlotInfo(EItemContainer::Loot, -1);
	if (Btn_Close)
	{
		Btn_Close->OnClicked.AddDynamic(this, &ThisClass::HandleCloseClicked);
	}
}

void UDZ_LootListPanel::SetContainerName(const FText& Name)
{
	Text_ContainerName->SetText(Name);
}

bool UDZ_LootListPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (!ScrollBox_LootList) return false;
	const FVector2D CursorPos = FSlateApplication::Get().GetCursorPos();
	if (!ScrollBox_LootList->GetCachedGeometry().IsUnderLocation(CursorPos))
	{
		return false;
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UDZ_LootListPanel::HandleLootRowItemDrop(UWidget* From, UWidget* To)
{
	LootItemMoveRequest.Broadcast(From, To);
}

void UDZ_LootListPanel::HandleLootRowTakeRequested(UWidget* Row)
{
	OnTakeRequested.Broadcast(Row);
}

void UDZ_LootListPanel::HandleCloseClicked()
{
	OnCloseRequested.Broadcast();
}

void UDZ_LootListPanel::SetLootListInfo(const TArray<FDZ_ItemStack>& ItemList)
{
	if (ItemList.IsEmpty())
	{
		ScrollBox_LootList->SetVisibility(ESlateVisibility::Collapsed);
		Text_EmptyHint->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	ScrollBox_LootList->SetVisibility(ESlateVisibility::Visible);
	Text_EmptyHint->SetVisibility(ESlateVisibility::Collapsed);
	
	int32 ChildrenCount = ScrollBox_LootList->GetChildrenCount();
	for (int32 i = 0; i < ItemList.Num() - ChildrenCount; i++)
	{
		UDZ_LootRow* NewItem = CreateWidget<UDZ_LootRow>(this, LootRowClass);
		ScrollBox_LootList->AddChild(NewItem);
		NewItem->SetSlotInfo(EItemContainer::Loot, i);
		NewItem->OnWidgetDragEnded.AddDynamic(this, &UDZ_LootListPanel::HandleLootRowItemDrop);
		NewItem->OnTakeRequested.AddDynamic(this, &UDZ_LootListPanel::HandleLootRowTakeRequested);
	}
	for (int32 i = ChildrenCount - 1; i >= ItemList.Num(); i--)
	{
		ScrollBox_LootList->GetChildAt(i)->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	for (int32 i = 0; i < ItemList.Num(); i++)
	{
		if (UDZ_LootRow* LootRow = Cast<UDZ_LootRow>(ScrollBox_LootList->GetChildAt(i)))
		{
			LootRow->SetItemInfo(ItemList[i].ItemId, ItemList[i].Count);
			LootRow->SetSlotInfo(EItemContainer::Loot, i);
			LootRow->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
