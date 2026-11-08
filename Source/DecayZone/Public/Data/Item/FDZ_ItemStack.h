#pragma once
#include "CoreMinimal.h"
#include "DZ_ItemDefines.h"
#include "FDZ_ItemStack.generated.h"

USTRUCT(BlueprintType)
struct FDZ_ItemStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ItemId = DZItem::EmptyId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count = 0;

	bool IsEmpty() const
	{
		return ItemId == DZItem::EmptyId || Count <= 0;
	}
};
