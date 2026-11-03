#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "FDZ_ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemUseType : uint8
{
	None,
	Heal,
	Throw,
	Equip,
	Buff
};

USTRUCT(BlueprintType)
struct FDZ_ItemData: public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ItemId = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ItemName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Weight = 0.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;
};
