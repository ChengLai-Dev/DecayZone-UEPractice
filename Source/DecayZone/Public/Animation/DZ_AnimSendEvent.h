// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DZ_AnimSendEvent.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_AnimSendEvent : public UAnimNotify
{
	GENERATED_BODY()
public:
	UDZ_AnimSendEvent();
	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;
	virtual FString GetNotifyName_Implementation() const override;
	
	UPROPERTY(EditAnywhere, Category = "DecayZone|GameplayTags")
	FGameplayTag EventTag;
};
