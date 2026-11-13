// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/DZ_AbilityEndReason.h"
#include "DZ_GameplayAbility.generated.h"


UCLASS()
class DECAYZONE_API UDZ_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DecayZone|Ability")
	void SetEndReason(EDZAbilityEndReason Reason, const FString& Detail = TEXT(""));

	UFUNCTION(BlueprintPure, Category = "DecayZone|Ability")
	EDZAbilityEndReason GetEndReason() const { return EndReason; }

	UFUNCTION(BlueprintPure, Category = "DecayZone|Ability")
	FString GetEndReasonDetail() const { return EndReasonDetail; }

protected:
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
		const FGameplayEventData* TriggerEventData = nullptr) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void OnAbilityEnded(EDZAbilityEndReason Reason) {}

private:
	EDZAbilityEndReason GetEndReason(bool bWasCancelled) const;
	
	EDZAbilityEndReason EndReason = EDZAbilityEndReason::Completed;
	
	FString EndReasonDetail;
};
