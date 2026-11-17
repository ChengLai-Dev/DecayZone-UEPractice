// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/HUD/DZ_HUDLayout.h"

#include "UI/Widgets/HUD/DZ_AbilityBar.h"
#include "UI/Widgets/HUD/DZ_RaidTime.h"
#include "UI/Widgets/HUD/DZ_StatusBars.h"

UDZ_HUDLayout::UDZ_HUDLayout()
{
	bResident = true;
}

UDZ_StatusBars* UDZ_HUDLayout::GetStatusBars() const
{
	return StatusBars;
}

UDZ_AbilityBar* UDZ_HUDLayout::GetAbilityBar() const
{
	return AbilityBar;
}

UDZ_RaidTime* UDZ_HUDLayout::GetRaidTime() const
{
	return RaidTime;
}
