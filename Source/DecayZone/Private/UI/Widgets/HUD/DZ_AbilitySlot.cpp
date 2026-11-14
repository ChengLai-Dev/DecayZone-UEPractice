// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/HUD/DZ_AbilitySlot.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace
{
	const TCHAR* CooldownPercentParameter = TEXT("Percent");
	constexpr float MinCooldownDisplaySeconds = 0.5f;
}

UDZ_AbilitySlot::UDZ_AbilitySlot()
{
}

void UDZ_AbilitySlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CooldownMaterialInstance = Img_CooldownMask->GetDynamicMaterial();
}

void UDZ_AbilitySlot::SetKeyText(const FText& KeyText)
{
	Text_Key->SetText(KeyText);
}

void UDZ_AbilitySlot::SetIcon(UTexture2D* Icon)
{
	if (bIsEmpty) return;

	if (CachedIcon != Icon)
	{
		CachedIcon = Icon;
		if (IsValid(Icon))
		{
			Img_Icon->SetBrushFromTexture(Icon);
			Img_Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Img_Icon->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UDZ_AbilitySlot::SetCooldown(float Percent, float RemainingSeconds)
{
	if (bIsEmpty) return;

	const bool bOnCooldown = Percent > UE_KINDA_SMALL_NUMBER && IsValid(CooldownMaterialInstance);
	if (!bOnCooldown)
	{
		Img_CooldownMask->SetVisibility(ESlateVisibility::Collapsed);
		Text_Cooldown->SetText(FText::GetEmpty());
		return;
	}

	CooldownMaterialInstance->SetScalarParameterValue(CooldownPercentParameter, FMath::Clamp(Percent, 0.f, 1.f));
	Img_CooldownMask->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (RemainingSeconds > MinCooldownDisplaySeconds)
	{
		Text_Cooldown->SetText(FText::AsNumber(FMath::CeilToInt(RemainingSeconds)));
	}
	else
	{
		Text_Cooldown->SetText(FText::GetEmpty());
	}
}

void UDZ_AbilitySlot::SetAvailable(bool bAvailable)
{
	if (bIsEmpty) return;

	Border_Disabled->SetVisibility(bAvailable ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible);
}

void UDZ_AbilitySlot::SetActive(bool bActive)
{
	if (bIsEmpty) return;

	Border_Active->SetVisibility(bActive ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}

void UDZ_AbilitySlot::SetEmpty(bool bEmpty)
{
	bIsEmpty = bEmpty;
	if (!bIsEmpty) return;

	CachedIcon = nullptr;
	Img_Icon->SetVisibility(ESlateVisibility::Collapsed);
	Img_CooldownMask->SetVisibility(ESlateVisibility::Collapsed);
	Border_Disabled->SetVisibility(ESlateVisibility::Collapsed);
	Border_Active->SetVisibility(ESlateVisibility::Collapsed);
	Text_Cooldown->SetText(FText::GetEmpty());
}
