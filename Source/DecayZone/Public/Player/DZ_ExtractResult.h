// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_ExtractResult.generated.h"

UENUM(BlueprintType)
enum class EDZExtractResult : uint8
{
	InProgress,
	Extracted,
	Dead,
};
