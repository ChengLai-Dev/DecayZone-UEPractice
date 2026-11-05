// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_InteractProgress.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDZ_InteractProgress::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CacheMaterial = Img_RingFill->GetDynamicMaterial();
}

void UDZ_InteractProgress::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bIsCounting)
	{
		LeftSeconds -= InDeltaTime;
		if (LeftSeconds <= 0)
		{
			CancelCountDown();
			OnProgressComplete.Broadcast();
			return;
		}
		UpdateProgress();
	}
}

void UDZ_InteractProgress::StartCountDownSeconds(float Seconds)
{
	if (Seconds <= 0)
	{
		OnProgressComplete.Broadcast();
		return;
	}
	
	bIsCounting = true;
	TotalSeconds = Seconds;
	LeftSeconds = Seconds;
	UpdateProgress();
	SetVisibility(ESlateVisibility::Visible);
}

void UDZ_InteractProgress::CancelCountDown()
{
	bIsCounting = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UDZ_InteractProgress::UpdateProgress()
{
	Text_Time->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FMath::Max(0.f, LeftSeconds))));
	if (IsValid(CacheMaterial))
	{
		CacheMaterial->SetScalarParameterValue("Progress", 1 - LeftSeconds / TotalSeconds);
	}
}

