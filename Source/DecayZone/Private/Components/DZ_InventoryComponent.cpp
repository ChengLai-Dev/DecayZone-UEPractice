// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DZ_InventoryComponent.h"


TArray<int32> UDZ_InventoryComponent::GetItems()
{
	return Items;
}

void UDZ_InventoryComponent::AddItem(int32 Item)
{
	Items.Add(Item);
}

void UDZ_InventoryComponent::RemoveItem(int32 Item)
{
	Items.Remove(Item);
}

