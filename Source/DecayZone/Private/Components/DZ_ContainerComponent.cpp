// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DZ_ContainerComponent.h"

#include "Dialogs/Dialogs.h"

UDZ_ContainerComponent::UDZ_ContainerComponent()
{
}

const TArray<FDZ_ItemStack>& UDZ_ContainerComponent::GetContents() const
{
	return Items;
}

int32 UDZ_ContainerComponent::TryTake(int32 SlotIndex, int32 Count)
{
	return 0;
}

void UDZ_ContainerComponent::SetContents(const TArray<FDZ_ItemStack>& InItems)
{
}

void UDZ_ContainerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void UDZ_ContainerComponent::OnRep_Items()
{
}

