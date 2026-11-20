// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "DZ_ListEntryWidget.generated.h"


UCLASS(Abstract)
class DECAYZONE_API UDZ_ListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void OnEntryRefresh(UObject* Item);
};
