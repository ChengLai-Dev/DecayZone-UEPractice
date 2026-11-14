// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Inventory/DZ_CharacterStats.h"

#include "Components/TextBlock.h"

void UDZ_CharacterStats::SetHealth(int32 Health, int32 MaxHealth)
{
	SetTextValue( Text_HealthValue, Health, MaxHealth);
}

void UDZ_CharacterStats::SetEnergy(int32 Energy, int32 MaxEnergy)
{
	SetTextValue(Text_EnergyValue, Energy, MaxEnergy);
}

void UDZ_CharacterStats::SetWeight(int32 Weight, int32 MaxWeight)
{
	SetTextValue(Text_WeightValue, Weight, MaxWeight);
}

void UDZ_CharacterStats::SetArmor(int32 Armor)
{
	Text_ArmorValue->SetText(FText::AsNumber(Armor));
}

void UDZ_CharacterStats::SetTextValue(UTextBlock* TextBlock, int32 Value, int32 MaxValue)
{
	const FText ValueText = FText::AsNumber(Value);
	const FText MaxMaxText = FText::AsNumber(MaxValue);
	TextBlock->SetText(FText::Format(FText::FromString(TEXT("{0}/{1}")), ValueText, MaxMaxText));
}
