// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"
#include "DZ_ListView.generated.h"

UCLASS()
class DECAYZONE_API UDZ_ListView : public UListView
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RefreshDisplayedEntries();
};
