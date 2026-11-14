// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_AbilitySlot.generated.h"

class UBorder;
class UImage;
class UMaterialInstanceDynamic;
class UTextBlock;
class UTexture2D;

UCLASS()
class DECAYZONE_API UDZ_AbilitySlot : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	UDZ_AbilitySlot();

	virtual void NativeOnInitialized() override;
	void SetKeyText(const FText& KeyText);
	void SetIcon(UTexture2D* Icon);
	void SetCooldown(float Percent, float RemainingSeconds);
	void SetAvailable(bool bAvailable);
	void SetActive(bool bActive);
	void SetEmpty(bool bEmpty);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_CooldownMask;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Disabled;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_Active;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Cooldown;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Key;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CooldownMaterialInstance;

	UPROPERTY()
	TObjectPtr<UTexture2D> CachedIcon;

	bool bIsEmpty = true;
};
