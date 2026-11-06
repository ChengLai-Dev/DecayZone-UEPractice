// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_InteractProgress.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_InteractProgress : public UDZ_UserWidget
{
	GENERATED_BODY()

public:
	UDZ_InteractProgress();
	
	virtual void NativeOnInitialized() override;
	void SetProgress(float Progress, float Remaining);

private:
	bool bIsCounting = false;
	float LeftSeconds = 0.f;
	float TotalSeconds = 0.f;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_RingFill;
		
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Time;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CacheMaterial;
};
