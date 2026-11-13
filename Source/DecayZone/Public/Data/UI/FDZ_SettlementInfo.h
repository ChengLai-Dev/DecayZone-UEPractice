#pragma once

#include "CoreMinimal.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "FDZ_SettlementInfo.generated.h"

/**
 * 
 * 
 */
USTRUCT(BlueprintType)
struct FDZ_SettlementInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSucceeded = false;

	UPROPERTY(BlueprintReadOnly)
	float ElapsedSeconds = 0.f;

	UPROPERTY(BlueprintReadOnly)
	int32 TotalValue = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 EnemyKills = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerKills = 0;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDZ_ItemStack> Items;
};
