// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_ItemSlotBase.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_ItemSlotBase : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	void SetSlotInfo(EItemContainer Container, int32 SlotIndex);
	FDZ_ItemSlotHandle GetSlotHandle();
	
protected:
	FDZ_ItemSlotHandle SlotHandle;
	
};
