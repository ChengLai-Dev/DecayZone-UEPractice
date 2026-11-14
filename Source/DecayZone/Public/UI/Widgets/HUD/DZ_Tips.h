// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_Tips.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_Tips : public UDZ_UserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeOnInitialized() override;
	
	void ShowTips(const FText& TipText, float InDuration = 3);
	
	
private:
	
	UFUNCTION()
	void HandleFadeInComplete();
	
	UFUNCTION()
	void HandleFadeOutComplete();
	
	void PlayFadeOutAnim();
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Tips;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeIn;
	
	UPROPERTY(Transient, meta=(BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_FadeOut;
	
	FTimerHandle HoldTimerHandle;
	float Duration;
};
