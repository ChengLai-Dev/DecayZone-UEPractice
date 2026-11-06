// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "Components/ActorComponent.h"
#include "DZ_InventoryComponent.generated.h"

struct FDZ_ItemStack;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DECAYZONE_API UDZ_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UDZ_InventoryComponent();
	virtual void BeginPlay() override;
	const TArray<FDZ_ItemStack>& GetItems();
	bool AddItem(int32 Index, int32 ItemID, int32 Count);
	void RemoveItem(int32 Index, int32 Count = 1);
	void RemoveItemsAll(int32 Index);
	bool MoveItem(int32 FromIndex, int32 ToIndex);
	bool AddItemAutoPos(int32 ItemID, int32 Count);
	bool IsSlotValid(int32 SlotIndex);
	
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnItemsChanged;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_Items)
	TArray<FDZ_ItemStack> Items;
	
	UPROPERTY(EditDefaultsOnly)
	int32 DefaultCapacity = 5;
	
	UFUNCTION()
	void OnRep_Items();
};
