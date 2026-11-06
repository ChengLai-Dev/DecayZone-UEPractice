// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_InteractProgress.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UDZ_InteractProgress::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	AddToViewport();
	Hide();
	CacheMaterial = Img_RingFill->GetDynamicMaterial();
}

void UDZ_InteractProgress::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UDZ_InteractProgress::Hide()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UDZ_InteractProgress::SetProgress(float Progress, float Remaining)
{
	Text_Time->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), FMath::Max(0.f, Remaining))));
	if (IsValid(CacheMaterial))
	{
		CacheMaterial->SetScalarParameterValue("Progress", Progress);
	}
}
