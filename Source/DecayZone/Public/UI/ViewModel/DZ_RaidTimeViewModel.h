// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "DZ_RaidTimeViewModel.generated.h"

UCLASS()
class DECAYZONE_API UDZ_RaidTimeViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void SetElapsedSeconds(float InSeconds);

	const FText& GetTimeText() const { return TimeText; }

private:
	void SetTimeText(const FText& TimeText);
	
	UPROPERTY(FieldNotify, BlueprintReadOnly, Category = "DecayZone|Raid", meta = (AllowPrivateAccess = "true"))
	FText TimeText;
};
