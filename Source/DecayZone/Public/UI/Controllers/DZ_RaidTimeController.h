// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_RaidTimeController.generated.h"

class UDZ_RaidTime;
class UDZ_RaidTimeViewModel;

UCLASS()
class DECAYZONE_API UDZ_RaidTimeController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	void SetRaidTimeWidget(UDZ_RaidTime* InWidget);
	virtual void Shutdown() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void RefreshTime();

	UPROPERTY()
	TObjectPtr<UDZ_RaidTime> RaidTimeWidget;

	UPROPERTY()
	TObjectPtr<UDZ_RaidTimeViewModel> ViewModel;
};
