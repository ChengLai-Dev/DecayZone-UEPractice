// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/DZ_RaidTimeViewModel.h"

void UDZ_RaidTimeViewModel::SetTimeText(const FText& InTimeText)
{
	UE_MVVM_SET_PROPERTY_VALUE(TimeText, InTimeText);
}

void UDZ_RaidTimeViewModel::SetElapsedSeconds(float InSeconds)
{
	const int32 TotalSeconds = FMath::Max(0, FMath::FloorToInt(InSeconds));
	const int32 Minutes = TotalSeconds / 60;
	const int32 Seconds = TotalSeconds % 60;
	const FText& Text = FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
	
	SetTimeText(Text);
}
 