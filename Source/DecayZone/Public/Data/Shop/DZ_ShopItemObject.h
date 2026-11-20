// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "UObject/Object.h"
#include "DZ_ShopItemObject.generated.h"

class UTexture2D;

UCLASS()
class DECAYZONE_API UDZ_ShopItemObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "DecayZone|Shop")
	int32 ItemId = DZItem::EmptyId;

	UPROPERTY(BlueprintReadOnly, Category = "DecayZone|Shop")
	FText Name;

	UPROPERTY(BlueprintReadOnly, Category = "DecayZone|Shop")
	FText TypeText;

	UPROPERTY(BlueprintReadOnly, Category = "DecayZone|Shop")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, Category = "DecayZone|Shop")
	int32 Price = 0;

	UPROPERTY(BlueprintReadOnly, Category = "DecayZone|Shop")
	bool bCanBuy = false;
};
