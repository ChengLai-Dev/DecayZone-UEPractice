// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "Components/ActorComponent.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "DZ_ContainerComponent.generated.h"


struct FDZ_ContainerSpawnItemInfo;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DECAYZONE_API UDZ_ContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDZ_ContainerComponent();
	virtual void BeginPlay() override;
	
	const TArray<FDZ_ItemStack>& GetContents() const;
	
	int32 TryTake(int32 SlotIndex, int32 Count);
	
	TArray<FDZ_ItemStack> GetItems();
	
	void SetContents(const TArray<FDZ_ItemStack>& InItems);
	
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnContentsChanged;
	
	bool IsSlotValid(int32 SlotIndex);
	void RemoveItemsAll(int32 SlotIndex);
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_Items)
	TArray<FDZ_ItemStack> Items;
	
	UFUNCTION()
	void OnRep_Items();
	
	const FDZ_ContainerSpawnItemInfo* GetSpawnItemInfo();

	void GenerateContainerItems();
};
