// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_SettlementStatCard.generated.h"

class UTextBlock;

UCLASS()
class DECAYZONE_API UDZ_SettlementStatCard : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	void SetStat(int32 Value, const FText& Label);
	void SetStat(float Value, const FText& Label);

private:
	void SetStatText(const FText& Value, const FText& Label);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StatValue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StatLabel;
};
