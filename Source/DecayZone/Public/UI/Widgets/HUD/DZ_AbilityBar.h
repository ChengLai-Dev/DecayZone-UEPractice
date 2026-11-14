// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_AbilityBar.generated.h"

class UDZ_AbilitySlot;
class UTexture2D;

/**
 * 技能栏：按槽位索引把显示数据分发给各个槽，不持有技能身份。
 */
UCLASS()
class DECAYZONE_API UDZ_AbilityBar : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	UDZ_AbilityBar();

	static constexpr int32 MaxSlots = 2;

	void SetKeyText(int32 SlotIndex, const FText& KeyText);
	void SetIcon(int32 SlotIndex, UTexture2D* Icon);
	void SetCooldown(int32 SlotIndex, float Percent, float RemainingSeconds);
	void SetAvailable(int32 SlotIndex, bool bAvailable);
	void SetActive(int32 SlotIndex, bool bActive);
	void SetEmpty(int32 SlotIndex, bool bEmpty);

private:
	UDZ_AbilitySlot* GetSlot(int32 SlotIndex) const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_AbilitySlot> AbilitySlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDZ_AbilitySlot> AbilitySlot_2;
};
