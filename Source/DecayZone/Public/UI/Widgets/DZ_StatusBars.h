// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_StatusBars.generated.h"

class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_StatusBars : public UDZ_UserWidget
{
	GENERATED_BODY()
public:
	UDZ_StatusBars();
	
	void SetHealthInfo(float Health, float MaxHealth);
	void SetEnergyInfo(float Energy, float MaxEnergy);
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_HealthValue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_EnergyValue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> Bar_Health;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> Bar_Energy;
};
