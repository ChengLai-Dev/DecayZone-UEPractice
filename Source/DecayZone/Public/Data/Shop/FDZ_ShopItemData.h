#pragma once

#include "CoreMinimal.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Engine/DataTable.h"
#include "FDZ_ShopItemData.generated.h"

USTRUCT(BlueprintType)
struct FDZ_ShopItemData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemId = DZItem::EmptyId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Price = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bEnabled = true;
};
