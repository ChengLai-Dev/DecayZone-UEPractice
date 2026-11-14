// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Inventory/DZ_WeightBar.h"

#include "Components/ProgressBar.h"

void UDZ_WeightBar::SetWeight(float Current, float Max)
{
	if (!Bar_Weight) return;
	if (Max <= 0.f)
	{
		Bar_Weight->SetPercent(0.f);
		return;
	}
	Bar_Weight->SetPercent(FMath::Clamp(Current / Max, 0.f, 1.f));
}
