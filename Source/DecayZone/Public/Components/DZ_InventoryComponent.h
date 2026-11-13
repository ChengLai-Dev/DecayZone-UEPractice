// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "Components/ActorComponent.h"
#include "DZ_InventoryComponent.generated.h"

enum class EEquipSlot : uint8;
struct FDZ_ItemStack;
struct FDZ_ItemSlotHandle;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DECAYZONE_API UDZ_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UDZ_InventoryComponent();
	virtual void BeginPlay() override;
	const TArray<FDZ_ItemStack>& GetItems();
	const TArray<int32>& GetEquipments();
	
	bool IsEquipSlotValid(int32 SlotIndex);
	void SetEquipments(int32 SlotIndex, int32 Item);
	void ClearEquipment(int32 SlotIndex);
	
	bool AddItem(int32 Index, int32 ItemID, int32 Count);
	void RemoveItem(int32 Index, int32 Count = 1);
	void RemoveItemsAll(int32 Index);
	bool MoveItem(const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To);
	bool AddItemAutoPos(int32 ItemID, int32 Count);
	bool IsSlotValid(int32 SlotIndex);
	float GetTotalWeight() const;
	float GetTotalValue() const;
	float GetMaxWeight() const;
	float GetTotalArmor() const;
	bool CanAddWeight(float Delta) const;
	void ClearAll();
	TArray<FDZ_ItemStack> GetAllItems();
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnItemsChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnEquipmentsChanged;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_Items)
	TArray<FDZ_ItemStack> Items;
	
	UPROPERTY(ReplicatedUsing = OnRep_Equipments)
	TArray<int32> Equipments;
	
	UPROPERTY(EditDefaultsOnly)
	int32 DefaultCapacity = 5;
	
	UPROPERTY(EditDefaultsOnly)
	float MaxWeight = 40;
	
	UFUNCTION()
	void OnRep_Items();
	
	UFUNCTION()
	void OnRep_Equipments();
};
