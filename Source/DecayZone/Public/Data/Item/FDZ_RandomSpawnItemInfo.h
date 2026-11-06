#pragma once
#include "CoreMinimal.h"
#include "FDZ_RandomSpawnItemInfo.generated.h" 

USTRUCT(BlueprintType)
struct FDZ_RandomSpawnItemInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	int32 ItemID = 0;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MinNum = 0;
	
	UPROPERTY(EditDefaultsOnly)
	int32 MaxNum = 0;
	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.01", ClampMax = "1"))
	float DropRate = 0.0f;
};