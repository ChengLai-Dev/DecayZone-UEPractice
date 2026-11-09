// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/DZ_Tips.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UDZ_Tips::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	FWidgetAnimationDynamicEvent FadeInComplete;
	FWidgetAnimationDynamicEvent FadeOutComplete;
	FadeInComplete.BindDynamic(this, &ThisClass::HandleFadeInComplete);
	FadeOutComplete.BindDynamic(this, &ThisClass::HandleFadeOutComplete);
	BindToAnimationFinished(Anim_FadeIn, FadeInComplete);
	BindToAnimationFinished(Anim_FadeOut, FadeOutComplete);
}

void UDZ_Tips::ShowTips(const FText& TipText, float InDuration)
{
	Duration = InDuration;
	Text_Tips->SetText(TipText);
	PlayAnimation(Anim_FadeIn);
}


void UDZ_Tips::HandleFadeInComplete()
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::PlayFadeOutAnim, Duration);
}

void UDZ_Tips::HandleFadeOutComplete()
{
	RemoveFromParent();
}

void UDZ_Tips::PlayFadeOutAnim()
{
	PlayAnimation(Anim_FadeOut);
}
