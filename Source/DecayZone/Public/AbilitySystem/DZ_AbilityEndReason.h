// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_AbilityEndReason.generated.h"

UENUM(BlueprintType)
enum class EDZAbilityEndReason : uint8
{
	None,
	Completed,
	Cancelled,
	Failed
};
