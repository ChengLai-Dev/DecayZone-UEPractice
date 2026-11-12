// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DZ_DelegateTypes.h"
#include "GameFramework/Character.h"
#include "Interface/DZ_Interactable.h"
#include "DZ_BaseCharacter.generated.h"

class FOnEvent;
class UDZ_StartupAbilities;

UCLASS()
class DECAYZONE_API ADZ_BaseCharacter : public ACharacter, public IAbilitySystemInterface, public IDZ_Interactable
{
	GENERATED_BODY()

public:
	ADZ_BaseCharacter();
	virtual bool CanInteract_Implementation() const override;
	virtual float GetInteractDuration_Implementation() const override;
	virtual FText GetInteractPromptText_Implementation() const override;
	virtual bool IsAutoInteract_Implementation() const override;
	virtual void OnInteractCompleted_Implementation(AActor* Interactor) override;
	UFUNCTION(BlueprintPure)
	bool IsAlive() const;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	void UseItem(int32 ItemID);
	bool IsFriendly(AActor* TargetActor);
	
	UFUNCTION()
	virtual void Death();
	
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnDeathEvent;
	
protected:
	void BindDeathHandle();
	
	void GiveStartupAbilities();
	
	virtual void ClientHandleDeath();

	UPROPERTY(ReplicatedUsing = OnRep_IsDead)
	bool bIsDead = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecayZone|Character")
	TObjectPtr<UAnimMontage> DeathMontage;
	
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecayZone|Character")
	TObjectPtr<UAnimMontage> HitReactMontage;
private:

	UFUNCTION()
	void OnRep_IsDead();
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDZ_StartupAbilities> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly)
	FText InteractPromptText;
	
	UPROPERTY(EditDefaultsOnly)
	float InteractDuration = 4.f;
};
