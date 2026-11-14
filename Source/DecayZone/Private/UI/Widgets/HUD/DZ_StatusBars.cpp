// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/HUD/DZ_StatusBars.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

UDZ_StatusBars::UDZ_StatusBars()
{
	bResident = true;
}

void UDZ_StatusBars::SetHealthInfo(float Health, float MaxHealth)
{
	const float Percent = MaxHealth > 0.f ? Health / MaxHealth : 0.f;
	Bar_Health->SetPercent(Percent);
	const FText ValueText = FText::AsNumber(FMath::RoundToInt(Health));
	const FText MaxMaxText = FText::AsNumber(FMath::RoundToInt(MaxHealth));
	Text_HealthValue->SetText(FText::Format(FText::FromString(TEXT("{0} / {1}")), ValueText, MaxMaxText));
}

void UDZ_StatusBars::SetEnergyInfo(float Energy, float MaxEnergy)
{
	const float Percent = MaxEnergy > 0.f ? Energy / MaxEnergy : 0.f;
	Bar_Energy->SetPercent(Percent);
	const FText ValueText = FText::AsNumber(FMath::RoundToInt(Energy));
	const FText MaxMaxText = FText::AsNumber(FMath::RoundToInt(MaxEnergy));
	Text_EnergyValue->SetText(FText::Format(FText::FromString(TEXT("{0} / {1}")), ValueText, MaxMaxText));
}
