// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/List/DZ_ListView.h"

#include "UI/Widgets/List/DZ_ListEntryWidget.h"

void UDZ_ListView::RefreshDisplayedEntries()
{
	for (UUserWidget* EntryWidget : GetDisplayedEntryWidgets())
	{
		UDZ_ListEntryWidget* ListEntry = Cast<UDZ_ListEntryWidget>(EntryWidget);
		if (!IsValid(ListEntry)) continue;

		ListEntry->OnEntryRefresh(GetListObjectFromEntry(*EntryWidget));
	}
}
