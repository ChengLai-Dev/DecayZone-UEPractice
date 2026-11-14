// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/Interaction/DZ_InteractPrompt.h"

#include "Components/TextBlock.h"


UDZ_InteractPrompt::UDZ_InteractPrompt()
{
	bResident = true;
}

void UDZ_InteractPrompt::SetKeyText(const FText& KeyText)
{
	Text_Key->SetText(KeyText);
}

void UDZ_InteractPrompt::SetPromptText(const FText& PromptText)
{
	Text_Action->SetText(PromptText);
}
