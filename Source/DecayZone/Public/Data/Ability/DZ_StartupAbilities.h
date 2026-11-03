// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DZ_StartupAbilities.generated.h"

class UGameplayAbility;
class ADZ_BaseCharacter;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FStartupAbilityInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ADZ_BaseCharacter> CharacterClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> Abilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bActivateOnGiven = true;
};

UCLASS()
class DECAYZONE_API UDZ_StartupAbilities : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Abilities")
	TArray<FStartupAbilityInfo> StartupAbilityInfos;
};
