// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_HUDLayout.generated.h"

class UDZ_AbilityBar;
class UDZ_RaidTime;
class UDZ_StatusBars;

UCLASS()
class DECAYZONE_API UDZ_HUDLayout : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	UDZ_HUDLayout();

	UDZ_StatusBars* GetStatusBars() const;
	UDZ_AbilityBar* GetAbilityBar() const;
	UDZ_RaidTime* GetRaidTime() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_StatusBars> StatusBars;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_AbilityBar> AbilityBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_RaidTime> RaidTime;
};
