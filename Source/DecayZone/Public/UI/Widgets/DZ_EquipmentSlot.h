// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_EquipmentSlot.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_EquipmentSlot : public UDZ_UserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_EquipName;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_EquipIcon;
};
