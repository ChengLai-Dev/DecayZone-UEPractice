// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DZ_AnimSendEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"

FString UDZ_AnimSendEvent::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		return EventTag.GetTagLeafName().ToString();
	}

	return TEXT("None");
}


UDZ_AnimSendEvent::UDZ_AnimSendEvent()
{
	bIsNativeBranchingPoint = true;
}

void UDZ_AnimSendEvent::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	const USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent;

	AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (OwnerActor && OwnerActor->Implements<UAbilitySystemInterface>())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, FGameplayEventData());
	}
}
