// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DZ_ContainerComponent.h"

#include "Data/Item/DZ_ContainerSpawnItemConfig.h"
#include "Game/DZ_GameInstance.h"
#include "Library/ItemHelper.h"
#include "Net/UnrealNetwork.h"

UDZ_ContainerComponent::UDZ_ContainerComponent()
{
	SetIsReplicatedByDefault(true);
}

void UDZ_ContainerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		GenerateContainerItems();
	}
}

const FDZ_ContainerSpawnItemInfo* UDZ_ContainerComponent::GetSpawnItemInfo()
{
	UDZ_ContainerSpawnItemConfig* Config = Cast<UDZ_GameInstance>(GetOwner()->GetGameInstance())->ContainerSpawnItemConfig;
	for (const FDZ_ContainerSpawnItemInfo& Info: Config->ContainerSpawnItemInfo)
	{
		if (GetOwner()->GetClass() == Info.ContainerActorClass)
		{
			return &Info;
		}
	}
	return nullptr;
}

void UDZ_ContainerComponent::GenerateContainerItems()
{
	TMap<int32, int32> ItemCounts;
	if (const FDZ_ContainerSpawnItemInfo* Info = GetSpawnItemInfo())
	{
		for (const FDZ_ItemStack ItemStack: Info->CertainItems)
		{
			if (ItemStack.IsEmpty()) continue;
			ItemCounts.FindOrAdd(ItemStack.ItemId) +=  ItemStack.Count;
		}
		for (const FDZ_RandomSpawnItemInfo& RandomInfo: Info->RandomItems)
		{
			if (RandomInfo.ItemID == DZItem::EmptyId) continue;
			if (FMath::FRand() >= RandomInfo.DropRate) continue;
			
			const int32 RandomNum = FMath::RandRange(RandomInfo.MinNum, RandomInfo.MaxNum);
			ItemCounts.FindOrAdd(RandomInfo.ItemID) += RandomNum;
		}
	}
	for (const TTuple<int32, int32>& P: ItemCounts)
	{
		int32 ItemID = P.Key;
		int32 Count = P.Value;
		int32 MaxStack = ItemHelper::GetMaxStack(this, ItemID);
		if (MaxStack <= 0) continue;
		while (Count > MaxStack)
		{
			Items.Add(FDZ_ItemStack(ItemID, MaxStack));
			Count -= MaxStack;
		}
		if (Count > 0)
		{
			Items.Add(FDZ_ItemStack(ItemID, Count));
		}
	}
}

const TArray<FDZ_ItemStack>& UDZ_ContainerComponent::GetContents() const
{
	return Items;
}

int32 UDZ_ContainerComponent::TryTake(int32 SlotIndex, int32 Count)
{
	return 0;
}

TArray<FDZ_ItemStack> UDZ_ContainerComponent::GetItems()
{
	return Items;
}

void UDZ_ContainerComponent::SetContents(const TArray<FDZ_ItemStack>& InItems)
{
	if (!GetOwner()->HasAuthority()) return;
	Items = InItems;
	OnContentsChanged.Broadcast();
}

bool UDZ_ContainerComponent::IsSlotValid(int32 SlotIndex)
{
	return SlotIndex >= 0 && SlotIndex < Items.Num();
}

void UDZ_ContainerComponent::RemoveItemsAll(int32 SlotIndex)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsSlotValid(SlotIndex)) return;
	Items.RemoveAt(SlotIndex);
}

void UDZ_ContainerComponent::RemoveItems(int32 SlotIndex, int32 Count)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsSlotValid(SlotIndex)) return;
	Items[SlotIndex].Count -= Count;
	if (Items[SlotIndex].Count <= 0)
	{
		Items.RemoveAt(SlotIndex);
	}
}

bool UDZ_ContainerComponent::AddItem(int32 ItemID, int32 Count)
{
	int32 MaxStack = ItemHelper::GetMaxStack(this, ItemID);
	for (FDZ_ItemStack& ItemStack: Items)
	{
		if (ItemStack.ItemId == ItemID && ItemStack.Count + Count <= MaxStack)
		{
			ItemStack.Count += Count;
			return true;
		}
	}
	Items.Add(FDZ_ItemStack(ItemID, Count));
	return true;
}

void UDZ_ContainerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDZ_ContainerComponent, Items);
}

void UDZ_ContainerComponent::OnRep_Items()
{
	OnContentsChanged.Broadcast();
}

