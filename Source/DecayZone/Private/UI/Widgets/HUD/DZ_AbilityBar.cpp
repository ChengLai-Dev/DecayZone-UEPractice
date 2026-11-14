// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/HUD/DZ_AbilityBar.h"

#include "UI/Widgets/HUD/DZ_AbilitySlot.h"

UDZ_AbilityBar::UDZ_AbilityBar()
{
	bResident = true;
}

void UDZ_AbilityBar::SetKeyText(int32 SlotIndex, const FText& KeyText)
{
	if (UDZ_AbilitySlot* AbilitySlot = GetSlot(SlotIndex))
	{
		AbilitySlot->SetKeyText(KeyText);
	}
}

void UDZ_AbilityBar::SetIcon(int32 SlotIndex, UTexture2D* Icon)
{
	if (UDZ_AbilitySlot* AbilitySlot = GetSlot(SlotIndex))
	{
		AbilitySlot->SetIcon(Icon);
	}
}

void UDZ_AbilityBar::SetCooldown(int32 SlotIndex, float Percent, float RemainingSeconds)
{
	if (UDZ_AbilitySlot* AbilitySlot = GetSlot(SlotIndex))
	{
		AbilitySlot->SetCooldown(Percent, RemainingSeconds);
	}
}

void UDZ_AbilityBar::SetAvailable(int32 SlotIndex, bool bAvailable)
{
	if (UDZ_AbilitySlot* AbilitySlot = GetSlot(SlotIndex))
	{
		AbilitySlot->SetAvailable(bAvailable);
	}
}

void UDZ_AbilityBar::SetActive(int32 SlotIndex, bool bActive)
{
	if (UDZ_AbilitySlot* AbilitySlot = GetSlot(SlotIndex))
	{
		AbilitySlot->SetActive(bActive);
	}
}

void UDZ_AbilityBar::SetEmpty(int32 SlotIndex, bool bEmpty)
{
	if (UDZ_AbilitySlot* AbilitySlot = GetSlot(SlotIndex))
	{
		AbilitySlot->SetEmpty(bEmpty);
	}
}

UDZ_AbilitySlot* UDZ_AbilityBar::GetSlot(int32 SlotIndex) const
{
	switch (SlotIndex)
	{
	case 0:
		return AbilitySlot_1;
	case 1:
		return AbilitySlot_2;
	default:
		return nullptr;
	}
}
