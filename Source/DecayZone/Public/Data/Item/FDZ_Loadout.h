#pragma once
#include "CoreMinimal.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "FDZ_Loadout.generated.h"

USTRUCT(BlueprintType)
struct FDZ_Loadout
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> Equipments;

	UPROPERTY(BlueprintReadOnly)
	TArray<FDZ_ItemStack> BagItems;
};
