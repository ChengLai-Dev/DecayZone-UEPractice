// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Shop/DZ_ShopEntryWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/Shop/DZ_ShopItemObject.h"
#include "UI/Widgets/Shop/DZ_ShopPanel.h"

void UDZ_ShopEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(Btn_Buy))
	{
		Btn_Buy->OnClicked.AddDynamic(this, &ThisClass::HandleBuyClicked);
	}
}

void UDZ_ShopEntryWidget::OnEntryRefresh(UObject* Item)
{
	const UDZ_ShopItemObject* ShopItem = Cast<UDZ_ShopItemObject>(Item);

	if (!ShopItem)
	{
		if (IsValid(Text_Name)) Text_Name->SetText(FText::GetEmpty());
		if (IsValid(Text_Type)) Text_Type->SetText(FText::GetEmpty());
		if (IsValid(Text_Price)) Text_Price->SetText(FText::GetEmpty());
		if (IsValid(Img_Icon)) Img_Icon->SetVisibility(ESlateVisibility::Collapsed);
		if (IsValid(Btn_Buy)) Btn_Buy->SetIsEnabled(false);
		return;
	}

	if (IsValid(Text_Name))
	{
		Text_Name->SetText(ShopItem->Name);
	}
	if (IsValid(Text_Type))
	{
		Text_Type->SetText(ShopItem->TypeText);
	}
	if (IsValid(Text_Price))
	{
		Text_Price->SetText(FText::FromString(FString::Printf(TEXT("$%d"), ShopItem->Price)));
	}
	if (IsValid(Img_Icon))
	{
		if (IsValid(ShopItem->Icon))
		{
			Img_Icon->SetBrushFromTexture(ShopItem->Icon);
			Img_Icon->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Img_Icon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	if (IsValid(Btn_Buy))
	{
		Btn_Buy->SetIsEnabled(ShopItem->bCanBuy);
	}
}

void UDZ_ShopEntryWidget::HandleBuyClicked()
{
	UDZ_ShopItemObject* ShopItem = GetListItem<UDZ_ShopItemObject>();
	if (!IsValid(ShopItem)) return;

	UDZ_ShopPanel* ShopPanel = GetTypedOuter<UDZ_ShopPanel>();
	if (!IsValid(ShopPanel)) return;

	ShopPanel->RequestBuy(ShopItem);
}
