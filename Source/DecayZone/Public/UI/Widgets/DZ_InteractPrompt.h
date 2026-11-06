// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_InteractPrompt.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_InteractPrompt : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	UDZ_InteractPrompt();
	void SetKeyText(const FText& KeyText);
	void SetPromptText(const FText& PromptText);
private:
	UPROPERTY(meta=(BindWidget)) 
	TObjectPtr<UTextBlock> Text_Key;
	
	UPROPERTY(meta=(BindWidget)) 
	TObjectPtr<UTextBlock> Text_Action;
};
