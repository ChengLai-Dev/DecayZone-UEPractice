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
	
	UFUNCTION(BlueprintPure)
	bool IsAlive();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void UseItem(int32 ItemID);
	bool IsFriendly(AActor* TargetActor);
	

	UFUNCTION()
	virtual void Death();
	
	virtual void ClientHandleDeath();
	
protected:
	void BindDeathHandle();
	
	void GiveStartupAbilities();
	
	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecayZone|Character")
	TObjectPtr<UAnimMontage> DeathMontage;
	
private:
	
	UFUNCTION()
	void OnRep_IsDead();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDZ_StartupAbilities> StartupAbilities;
};
