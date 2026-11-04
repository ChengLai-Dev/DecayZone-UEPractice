// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DZ_UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

FReply UDZ_UserWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FReply Reply = FReply::Handled().CaptureMouse(TakeWidget());
		if (bEnableDragging)
		{
			Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
		}
		return Reply;
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UDZ_UserWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UDZ_UserWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!HasMouseCapture())
	{
		return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnWidgetRightClicked.Broadcast(this);
		return FReply::Handled().ReleaseMouseCapture();
	}
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (!bIsDragging)
		{
			OnWidgetLeftClicked.Broadcast(this);
		}
		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Handled().ReleaseMouseCapture();
}

void UDZ_UserWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                          UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	bIsDragging = true;
	OutOperation = NewObject<UDragDropOperation>();
	OutOperation->Payload = this;
	if (UDZ_UserWidget* VisualWidget = CreateDragVisualWidget())
	{
		OutOperation->DefaultDragVisual = VisualWidget;
		OutOperation->Pivot = EDragPivot::CenterCenter;
	}
	OnWidgetDragStart.Broadcast(this);
}

void UDZ_UserWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	bIsDragging = false;
	OnWidgetDragEnded.Broadcast(this, nullptr);
}

bool UDZ_UserWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (UDZ_UserWidget* Source = Cast<UDZ_UserWidget>(InOperation->Payload))
	{
		Source->bIsDragging = false;
		Source->OnWidgetDragEnded.Broadcast(Source, this);
	}
	return true;
}

UDZ_UserWidget* UDZ_UserWidget::CreateDragVisualWidget()
{
	return nullptr;
}

void UDZ_UserWidget::SetDragEnabled(bool bEnable)
{
	bEnableDragging = bEnable;
}
