// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_ItemSlotBase.h"

void UDZ_ItemSlotBase::SetSlotInfo(EItemContainer Container, int32 SlotIndex)
{
	SlotHandle.Container = Container;
	SlotHandle.SlotIndex = SlotIndex;
}

FDZ_ItemSlotHandle UDZ_ItemSlotBase::GetSlotHandle()
{
	return SlotHandle;
}