// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "DZ_BaseCharacter.generated.h"

class UDZ_StartupAbilities;

UCLASS()
class DECAYZONE_API ADZ_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADZ_BaseCharacter();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	void UseItem(int32 ItemID);
	
protected:
	
	void GiveStartupAbilities();
	
private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDZ_StartupAbilities> StartupAbilities;
};
