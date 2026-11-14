// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_StatusBarsController.generated.h"

class UDZ_StatusBars;
class UDZ_AbilitySystemComponent;

UCLASS()
class DECAYZONE_API UDZ_StatusBarsController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(ADZ_PlayerController* InPlayerController) override;
	virtual void Shutdown() override;

	void Refresh();

private:
	UDZ_AbilitySystemComponent* GetAbilitySystemComponent() const;

	UPROPERTY()
	TObjectPtr<UDZ_StatusBars> StatusBarsWidget;
};
