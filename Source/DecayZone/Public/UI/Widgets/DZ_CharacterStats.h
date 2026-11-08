// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_CharacterStats.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_CharacterStats : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	void SetHealth(int32 Health, int32 MaxHealth);
	void SetEnergy(int32 Energy, int32 MaxEnergy);
	void SetWeight(int32 Weight, int32 MaxWeight);
	void SetArmor(int32 Armor);
	
private:
	
	void SetTextValue(UTextBlock* TextBlock, int32 Value, int32 MaxValue);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_HealthValue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_EnergyValue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ArmorValue;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_WeightValue;
	
};
