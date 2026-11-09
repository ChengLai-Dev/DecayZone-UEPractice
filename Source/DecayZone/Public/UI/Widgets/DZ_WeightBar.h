// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_WeightBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_WeightBar : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	void SetWeight(float Current, float Max);
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> Bar_Weight;
};
