// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/DZ_ItemActor.h"

#include "Library/ItemHelper.h"

ADZ_ItemActor::ADZ_ItemActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

const FDZ_ItemData* ADZ_ItemActor::GetData() const
{
	return ItemHelper::GetItemData(this, ItemID);
}
