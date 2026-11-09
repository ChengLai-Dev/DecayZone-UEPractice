// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_BackpackPanel.h"

#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "Library/UIHelper.h"
#include "UI/Widgets/DZ_ItemSlot.h"
#include "UI/Widgets/DZ_WeightBar.h"

void UDZ_BackpackPanel::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDZ_BackpackPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UUniformGridPanel* Grid_Backpack = Cast<UUniformGridPanel>(UIHelper::SeekWidget(GetRootWidget(), "VB_Backpack.Grid_Backpack"));
	for (UWidget* Child: Grid_Backpack->GetAllChildren())
	{
		if (Child->GetName().StartsWith("ItemSlot_"))
		{
			if (UDZ_ItemSlot* ItemSlot = Cast<UDZ_ItemSlot>(Child))
			{
				ItemSlots.Add(ItemSlot);
				ItemSlot->SetSlotInfo(EItemContainer::Backpack, ItemSlots.Num() - 1);
				ItemSlot->OnWidgetDragEnded.AddDynamic(this, &ThisClass::HandleItemSlotDragEnded);
				ItemSlot->OnWidgetLeftClicked.AddDynamic(this, &ThisClass::HandleItemSlotClicked);
			}
		}
	}
}

void UDZ_BackpackPanel::SetBackpackPanel(const TArray<FDZ_ItemStack>& BackpackInfo)
{
	int32 i = 0;
	for (; i < BackpackInfo.Num(); i++)
	{
		const FDZ_ItemStack& Info = BackpackInfo[i];
		ItemSlots[i]->SetItemInfo(Info.ItemId, Info.Count);
		ItemSlots[i]->Show();
	}
	for (; i < ItemSlots.Num(); i++)
	{
		ItemSlots[i]->Hide();
	}
}

void UDZ_BackpackPanel::SetWeightInfo(float Current, float Max)
{
	Text_WeightValue->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), Current, Max)));
	WeightBar->SetWeight(Current, Max);
}

void UDZ_BackpackPanel::HandleItemSlotClicked(UWidget* Widget)
{
	OnItemSlotClicked.Broadcast(Cast<UDZ_ItemSlot>(Widget));
}

void UDZ_BackpackPanel::HandleItemSlotDragEnded(UWidget* From, UWidget* To)
{
	OnItemDrop.Broadcast(From, To);
}
