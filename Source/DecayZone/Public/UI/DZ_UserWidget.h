// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DZ_UserWidget.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetLeftClicked, UWidget*, Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetRightClicked, UWidget*, Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetDragStart, UWidget*, Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetDragging, UWidget*, Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetDragEnded, UWidget*, Widget);

UCLASS()
class DECAYZONE_API UDZ_UserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetLeftClicked OnWidgetLeftClicked;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetRightClicked OnWidgetRightClicked;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragStart OnWidgetDragStart;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragging OnWidgetDragging;
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEnded OnWidgetDragEnded;
	
private:
	bool bIsDragging = false;
};
