// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_AbilityBarController.generated.h"

class UDZ_AbilityBar;
class UDZ_AbilitySystemComponent;

UCLASS()
class DECAYZONE_API UDZ_AbilityBarController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(ADZ_PlayerController* InPlayerController) override;
	virtual void Shutdown() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetSlotSkill(int32 SlotIndex, int32 SkillId);
	int32 GetSlotSkill(int32 SlotIndex) const;
	void Refresh();
	void Hide();

private:
	void RefreshSlotsStatic();
	void RefreshSlotsDynamic();
	void RefreshSlotStatic(int32 SlotIndex);
	void RefreshSlotDynamic(int32 SlotIndex);
	UDZ_AbilitySystemComponent* GetAbilitySystemComponent() const;

	UPROPERTY()
	TObjectPtr<UDZ_AbilityBar> AbilityBarWidget;

	float RefreshAccum = 0.f;
};
