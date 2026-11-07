// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DZ_InventoryComponent.h"

#include "Data/Item/FDZ_ItemStack.h"
#include "Game/DZ_GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"

void UDZ_InventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, Items);
	DOREPLIFETIME(ThisClass, Equipments);
}

UDZ_InventoryComponent::UDZ_InventoryComponent()
{
	SetIsReplicatedByDefault(true);
	for (int32 Index = 0; Index < DefaultCapacity; Index++)
	{
		Items.Add(FDZ_ItemStack());
	}
	for (int32 Index = 0; Index < static_cast<int32>(EEquipSlot::None); Index++)
	{
		Equipments.Add(-1);
	}
}

void UDZ_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->HasAuthority()) return;

	UDZ_GameInstance* GI = Cast<UDZ_GameInstance>(GetWorld()->GetGameInstance());
	int32 SlotIndex = 0;
	for (int32 ItemId = 0; ItemId <= 10 && SlotIndex < 4; ItemId++)
	{
		if (GI->GetItemData(ItemId))
		{
			AddItem(SlotIndex, ItemId, 3);
			SlotIndex++;
		}
	}
}

const TArray<FDZ_ItemStack>& UDZ_InventoryComponent::GetItems()
{
	return Items;
}

const TArray<int32>& UDZ_InventoryComponent::GetEquipments()
{
	return Equipments;
}

bool UDZ_InventoryComponent::IsEquipSlotValid(int32 SlotIndex)
{
	return SlotIndex >= 0 && SlotIndex < Equipments.Num();
}

void UDZ_InventoryComponent::SetEquipments(int32 SlotIndex, int32 Item)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsEquipSlotValid(SlotIndex)) return;
	
	Equipments[SlotIndex] = Item;
}

void UDZ_InventoryComponent::ClearEquipment(int32 SlotIndex)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsEquipSlotValid(SlotIndex)) return;
	
	Equipments[SlotIndex] = -1;
}

bool UDZ_InventoryComponent::AddItem(int32 Index, int32 ItemID, int32 Count)
{
	if (!GetOwner()->HasAuthority()) return false;
	if (Index < 0 || Index >= Items.Num()) return false;
	if (Items[Index].Count == 0)
	{
		Items[Index].ItemId = ItemID;
		Items[Index].Count = Count;
		return true;
	}
	
	if (Items[Index].ItemId == ItemID)
	{
		Items[Index].Count += Count;
		return true;
	}
	
	return AddItemAutoPos(ItemID, Count);
}

void UDZ_InventoryComponent::RemoveItem(int32 Index, int32 Count)
{
	if (!GetOwner()->HasAuthority()) return;
	if (Index < 0 || Index >= Items.Num()) return;
	Items[Index].Count = FMath::Clamp(Items[Index].Count - Count, 0, Items[Index].Count);
}

void UDZ_InventoryComponent::RemoveItemsAll(int32 Index)
{
	if (!GetOwner()->HasAuthority()) return;
	if (Index < 0 || Index >= Items.Num()) return;
	Items[Index].Count = 0;
}

bool UDZ_InventoryComponent::MoveItem(const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To)
{
	if (!GetOwner()->HasAuthority()) return false;
	int32 FromIndex = From.SlotIndex;
	int32 ToIndex = To.SlotIndex;
	// 背包->背包
	if (From.Container == EItemContainer::Backpack && To.Container == EItemContainer::Backpack)
	{
		if (FromIndex < 0 || FromIndex >= Items.Num()) return false;
		if (ToIndex < 0 || ToIndex >= Items.Num()) return false;
	
		if (Items[FromIndex].ItemId != Items[ToIndex].ItemId)
		{
			Swap(Items[FromIndex] ,Items[ToIndex]);
			return true;
		}
		AddItem(ToIndex, Items[FromIndex].ItemId, Items[FromIndex].Count);
		RemoveItemsAll(FromIndex);
		return true;
	}
	// 装备->背包
	if (From.Container == EItemContainer::Equipment && To.Container == EItemContainer::Backpack)
	{
		if (!IsSlotValid(ToIndex)) return false;
		if (!IsEquipSlotValid(FromIndex)) return false;

		AddItem(ToIndex, Equipments[FromIndex], 1);
		ClearEquipment(FromIndex);
		return true;
	}
	// 背包->装备
	if (From.Container == EItemContainer::Backpack && To.Container == EItemContainer::Equipment)
	{
		if (!IsSlotValid(FromIndex)) return false;
		if (!IsEquipSlotValid(ToIndex)) return false;

		if (Equipments[ToIndex] != -1)
		{
			Swap(Equipments[ToIndex], Items[FromIndex].ItemId);
		}
		else
		{
			SetEquipments(ToIndex, Items[FromIndex].ItemId);
			RemoveItem(FromIndex);
		}
		return true;
	}
	return false;
}

bool UDZ_InventoryComponent::AddItemAutoPos(int32 ItemID, int32 Count)
{
	if (!GetOwner()->HasAuthority()) return false;
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index].ItemId == ItemID)
		{
			Items[Index].Count += Count;
			return true;
		}
	}
	for (int32 Index = 0; Index < Items.Num(); Index++)
	{
		if (Items[Index].Count == 0)
		{
			Items[Index].ItemId = ItemID;
			Items[Index].Count = Count;
			return true;
		}
	}
	return false;
}

bool UDZ_InventoryComponent::IsSlotValid(int32 SlotIndex)
{
	return SlotIndex >= 0 && SlotIndex < Items.Num();
}

void UDZ_InventoryComponent::OnRep_Items()
{
	OnItemsChanged.Broadcast();	
}

void UDZ_InventoryComponent::OnRep_Equipments()
{
	OnItemsChanged.Broadcast();
}
