// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DZ_InventoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DECAYZONE_API UDZ_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	TArray<int32> GetItems();
	
	void AddItem(int32 Item);
	void RemoveItem(int32 Item);
	
private:
	
	UPROPERTY()
	TArray<int32> Items;
};
