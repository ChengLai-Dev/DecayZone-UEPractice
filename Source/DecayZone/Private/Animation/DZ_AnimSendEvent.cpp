// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/DZ_AnimSendEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueManager.h"

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
	if (!OwnerActor) return;
	
	if (CosmeticCueTag.IsValid())
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Instigator = OwnerActor;
		FTransform SpawnTransform;
		if (FindCosmeticTransform(MeshComp, SpawnTransform))
		{
			CueParameters.Location = SpawnTransform.GetLocation();
			CueParameters.Normal = SpawnTransform.GetRotation().GetForwardVector();
		}
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
			OwnerActor, CosmeticCueTag, EGameplayCueEvent::Executed, CueParameters);
		
	}
	
	if (OwnerActor->Implements<UAbilitySystemInterface>())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, EventTag, FGameplayEventData());
	}
}

bool UDZ_AnimSendEvent::FindCosmeticTransform(const USkeletalMeshComponent* MeshComp, FTransform& OutTransform) const
{
	const AActor* OwnerActor = MeshComp ? MeshComp->GetOwner() : nullptr;
	if (!OwnerActor) return false;

	TArray<AActor*> AttachedActors;
	OwnerActor->GetAttachedActors(AttachedActors);

	for (const AActor* Attached : AttachedActors)
	{
		TArray<USceneComponent*> Components;
		Attached->GetComponents<USceneComponent>(Components);

		for (const USceneComponent* Component : Components)
		{
			if (!Component || !Component->DoesSocketExist(CosmeticSocketName)) continue;

			OutTransform = Component->GetSocketTransform(CosmeticSocketName);
			return true;
		}
	}

	if (!MeshComp) return false;

	if (MeshComp->DoesSocketExist(CosmeticSocketName))
	{
		OutTransform = MeshComp->GetSocketTransform(CosmeticSocketName);
		return true;
	}

	OutTransform = MeshComp->GetComponentTransform();
	return true;
}