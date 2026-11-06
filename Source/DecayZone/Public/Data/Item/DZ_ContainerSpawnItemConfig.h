// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/Item/FDZ_RandomSpawnItemInfo.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "DZ_ContainerSpawnItemConfig.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDZ_ContainerSpawnItemInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ContainerActorClass;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FDZ_RandomSpawnItemInfo> RandomItems;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FDZ_ItemStack> CertainItems;
};

UCLASS()
class DECAYZONE_API UDZ_ContainerSpawnItemConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FDZ_ContainerSpawnItemInfo> ContainerSpawnItemInfo;
};
