// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "Blueprint/UserWidget.h"
#include "DZ_UserWidget.generated.h"

/**
 * 
 */

UCLASS()
class DECAYZONE_API UDZ_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual UDZ_UserWidget* CreateDragVisualWidget();
	void SetDragEnabled(bool bEnable);
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnWidgetLeftClicked;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnWidgetRightClicked;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnWidgetDragStart;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetEvent OnWidgetDragging;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnWidgetDragEnded;
	
private:
	bool bIsDragging = false;
	bool bEnableDragging = false;
};
