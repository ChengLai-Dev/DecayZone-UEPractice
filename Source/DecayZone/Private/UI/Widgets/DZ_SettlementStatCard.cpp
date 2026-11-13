// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_SettlementStatCard.h"

#include "Components/TextBlock.h"

void UDZ_SettlementStatCard::SetStat(int32 Value, const FText& Label)
{
	SetStatText(FText::AsNumber(Value), Label);
}

void UDZ_SettlementStatCard::SetStat(float Value, const FText& Label)
{
	SetStatText(FText::FromString(FString::Printf(TEXT("%.1f"), Value)), Label);
}

void UDZ_SettlementStatCard::SetStatText(const FText& Value, const FText& Label)
{
	if (Text_StatValue)
	{
		Text_StatValue->SetText(Value);
	}
	if (Text_StatLabel)
	{
		Text_StatLabel->SetText(Label);
	}
}
