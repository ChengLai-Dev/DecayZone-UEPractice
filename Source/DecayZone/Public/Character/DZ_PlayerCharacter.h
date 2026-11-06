// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "DZ_BaseCharacter.h"
#include "DZ_DelegateTypes.h"
#include "DZ_PlayerCharacter.generated.h"

class UDZ_InventoryComponent;
class UCameraComponent;
class USpringArmComponent;
class UDZ_AbilitySystemComponent;
class UBoxComponent;

UCLASS()
class DECAYZONE_API ADZ_PlayerCharacter : public ADZ_BaseCharacter
{
	GENERATED_BODY()

public:
	ADZ_PlayerCharacter();
	
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void PossessedBy(AController* NewController) override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void OnRep_PlayerState() override;

	UFUNCTION()
	void OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	AActor* GetHighlightActor();
	
	UPROPERTY(BlueprintAssignable)
	FOnActorEvent OnFocusInteractableActorChanged;
	
	bool IsInteractableInRange(AActor* Actor);
	
protected:
	virtual void BeginPlay() override;

private:
	void HighlightInteractable();
	AActor* FindTargetInteractableActor();

	UPROPERTY()
	TSet<AActor*> OverlappingInteractables;
	
	UPROPERTY()
	TObjectPtr<UDZ_AbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY()
	TObjectPtr<UDZ_InventoryComponent> InventoryComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> InteractionBox;
	
	TWeakObjectPtr<AActor> CurrentInteractableActor;
};
