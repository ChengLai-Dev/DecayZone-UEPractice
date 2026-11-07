// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UIManager.generated.h"

class UDZ_UserWidget;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UUIManager : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	void OpenWidget(UDZ_UserWidget* Widget, bool bResident=false);
	void CloseWidget(UDZ_UserWidget* Widget);
	void CloseTopWidget();
	void UpdateInputMode();
private:
	TArray<TWeakObjectPtr<UDZ_UserWidget>> CurrentActivateWidgets;
};
