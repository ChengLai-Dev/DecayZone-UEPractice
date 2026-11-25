// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DZ_BaseCharacter.h"
#include "DZ_DelegateTypes.h"
#include "GameplayEffectTypes.h"
#include "DZ_PlayerCharacter.generated.h"

class UDZ_ContainerComponent;
class UGameplayEffect;
class UCameraComponent;
class USpringArmComponent;
class UDZ_AbilitySystemComponent;
class UBoxComponent;
class UWeaponData;

UCLASS()
class DECAYZONE_API ADZ_PlayerCharacter : public ADZ_BaseCharacter
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	ADZ_PlayerCharacter();
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual void OnRep_PlayerState() override;

	UFUNCTION()
	void OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	AActor* GetHighlightActor();
	
	UPROPERTY(BlueprintAssignable)
	FOnActorEvent OnFocusInteractableActorChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnEvent OnASCInitialized;

	UPROPERTY(BlueprintAssignable)
	FOnEvent OnAttributesInitialized;
	
	bool IsInteractableInRange(AActor* Actor);
	
	int32 GetActiveWeapon();
	
	UFUNCTION(BlueprintPure)
	const UWeaponData* GetActiveWeaponData() const;
	
	FName GetWeaponSocketName();
	
	void SetActiveWeapon(int32 Weapon);
	
protected:
	virtual void OnAbilitySystemInitialized() override;
	
private:
	
	UPROPERTY(ReplicatedUsing = OnRep_ActiveWeaponSlot)
	int32 ActiveWeaponSlot;
	
	UFUNCTION()
	void OnRep_ActiveWeaponSlot();
	
	virtual void Death() override;
	void HighlightInteractable();
	AActor* FindTargetInteractableActor();
	
	void RecalculateArmor();
	
	UFUNCTION()
	void HandleEquipmentsChanged();

	UPROPERTY()
	TSet<AActor*> OverlappingInteractables;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> InteractionBox;
	
	TWeakObjectPtr<AActor> CurrentInteractableActor;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone")
	TSubclassOf<UGameplayEffect> EquipmentArmorGEClass;
	
	FActiveGameplayEffectHandle ArmorGEHandle;
	
	UPROPERTY()
	TObjectPtr<UDZ_ContainerComponent> ContainerComponent;
};
