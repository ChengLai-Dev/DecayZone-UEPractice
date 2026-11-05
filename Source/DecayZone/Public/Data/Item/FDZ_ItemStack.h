#pragma once
#include "CoreMinimal.h"
#include "FDZ_ItemStack.generated.h"

USTRUCT(BlueprintType)
struct FDZ_ItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemId = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count = 0;
};