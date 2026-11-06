// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_LootListPanel.h"

#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "UI/Widgets/DZ_LootRow.h"

void UDZ_LootListPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ScrollBox_LootList->ClearChildren();
}

void UDZ_LootListPanel::SetContainerName(const FText& Name)
{
	Text_ContainerName->SetText(Name);
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
		UUserWidget* NewItem = CreateWidget<UUserWidget>(this, LootRowClass);
		ScrollBox_LootList->AddChild(NewItem);
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
			LootRow->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
