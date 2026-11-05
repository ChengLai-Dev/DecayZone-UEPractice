// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_BackpackPanel.h"

#include "Components/UniformGridPanel.h"
#include "Library/UIHelper.h"
#include "UI/Widgets/DZ_ItemSlot.h"

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
				ItemSlot->SetItemInfo(2);
				ItemSlot->OnWidgetLeftClicked.AddDynamic(this, &ThisClass::HandleItemSlotClicked);
			}
		}
	}
}

void UDZ_BackpackPanel::HandleItemSlotClicked(UWidget* Widget)
{
	OnItemSlotClicked.Broadcast(Cast<UDZ_ItemSlot>(Widget));
}
