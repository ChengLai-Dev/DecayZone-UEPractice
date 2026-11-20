// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Shop/DZ_ShopPanel.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Data/Shop/DZ_ShopItemObject.h"
#include "UI/Widgets/List/DZ_ListView.h"

void UDZ_ShopPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(Btn_Close))
	{
		Btn_Close->OnClicked.AddDynamic(this, &ThisClass::HandleCloseClicked);
	}
}

void UDZ_ShopPanel::SetMoney(int32 Money)
{
	if (!IsValid(Text_Money)) return;

	Text_Money->SetText(FText::FromString(FString::Printf(TEXT("$%d"), Money)));
}

void UDZ_ShopPanel::SetShopItems(const TArray<TObjectPtr<UDZ_ShopItemObject>>& InItems)
{
	Items.Reset();

	TArray<UObject*> RawItems;
	RawItems.Reserve(InItems.Num());
	for (const TObjectPtr<UDZ_ShopItemObject>& Item : InItems)
	{
		if (!IsValid(Item)) continue;
		Items.Add(Item);
		RawItems.Add(Item);
	}

	if (!IsValid(List_Items)) return;
	List_Items->SetListItems(RawItems);
}

void UDZ_ShopPanel::RefreshEntries()
{
	if (!IsValid(List_Items)) return;

	List_Items->RefreshDisplayedEntries();
}

void UDZ_ShopPanel::RequestBuy(UDZ_ShopItemObject* ShopItem)
{
	if (!IsValid(ShopItem)) return;

	OnBuyRequested.Broadcast(ShopItem->ItemId);
}

void UDZ_ShopPanel::HandleCloseClicked()
{
	Close();
}
