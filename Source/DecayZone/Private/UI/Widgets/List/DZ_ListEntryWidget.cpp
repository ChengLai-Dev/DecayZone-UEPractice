// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/List/DZ_ListEntryWidget.h"

void UDZ_ListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	OnEntryRefresh(ListItemObject);
}

void UDZ_ListEntryWidget::OnEntryRefresh(UObject* Item)
{
}
