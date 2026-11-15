// Fill out your copyright notice in the Description page of Project Settings.


#include "DecayZone/Public/Character/DZ_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Components/DZ_ContainerComponent.h"
#include "Data/Ability/DZ_StartupAbilities.h"
#include "Game/DZ_GameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "Library/DZ_BlueprintLibrary.h"
#include "Library/ItemHelper.h"
#include "Net/UnrealNetwork.h"
#include "Player/DZ_PlayerController.h"

ADZ_BaseCharacter::ADZ_BaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ADZ_BaseCharacter::CanInteract_Implementation() const
{
	return bIsDead;
}

float ADZ_BaseCharacter::GetInteractDuration_Implementation() const
{
	return InteractDuration;
}

FText ADZ_BaseCharacter::GetInteractPromptText_Implementation() const
{
	return InteractPromptText;
}

bool ADZ_BaseCharacter::IsAutoInteract_Implementation() const
{
	return false;
}

void ADZ_BaseCharacter::OnInteractCompleted_Implementation(AActor* Interactor, bool bSucceeded)
{
	if (!bSucceeded) return;

	APawn* Pawn = Cast<APawn>(Interactor);
	if (!Pawn) return;

	ADZ_PlayerController* PlayerController = Cast<ADZ_PlayerController>(Pawn->GetController());
	if (!PlayerController) return;
	
	if (UDZ_ContainerComponent* Container = FindComponentByClass<UDZ_ContainerComponent>())
	{
		PlayerController->ClientOpenLootPanel(Container);
	}
}

void ADZ_BaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, bIsDead);
}

UAbilitySystemComponent* ADZ_BaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADZ_BaseCharacter::SetupAbilitySystem(UAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	AbilitySystemComponent = Cast<UDZ_AbilitySystemComponent>(InASC);
	if (!AbilitySystemComponent) return;
	
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, this);
	
	if (bAbilitySystemInitialized) return;
	bAbilitySystemInitialized = true;
	
	BindAbilitySystemDelegates();
	
	if (HasAuthority())
	{
		ApplyInitialGameplayEffects();
		GiveStartupAbilities();
		BindDeathHandle();
	}
	
	OnAbilitySystemInitialized();
}

void ADZ_BaseCharacter::ApplyInitialGameplayEffects()
{
	if (!AbilitySystemComponent) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialGEClasses)
	{
		if (!EffectClass) continue;
		
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1, AbilitySystemComponent->MakeEffectContext());
		if (!SpecHandle.IsValid()) continue;
		
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	}
}

void ADZ_BaseCharacter::BindAbilitySystemDelegates()
{
	if (!AbilitySystemComponent) return;
	
	MoveSpeedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UDZ_AttributeSet::GetMoveSpeedAttribute()).AddUObject(this, &ThisClass::OnMoveSpeedChanged);
	
	// GE由服务端应用，服务端复制属性到客户端时，客户端可能还未绑定漏掉通知，所以直接设置一次值
	const float CurrentMoveSpeed = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMoveSpeedAttribute());
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (CurrentMoveSpeed > 0.f && MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = CurrentMoveSpeed;
	}
}

void ADZ_BaseCharacter::UnbindAbilitySystemDelegates()
{
	if (!AbilitySystemComponent) return;
	if (!MoveSpeedDelegateHandle.IsValid()) return;
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UDZ_AttributeSet::GetMoveSpeedAttribute()).Remove(MoveSpeedDelegateHandle);
	MoveSpeedDelegateHandle.Reset();
}

void ADZ_BaseCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f) return;
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Data.NewValue;
	}
}

void ADZ_BaseCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindAbilitySystemDelegates();
	
	Super::EndPlay(EndPlayReason);
}

void ADZ_BaseCharacter::UseItem(int32 ItemID)
{
	ItemHelper::UseItem(this, ItemID);
}

bool ADZ_BaseCharacter::IsFriendly(AActor* TargetActor)
{
	return UDZ_BlueprintLibrary::IsFriendly(this, TargetActor);
}

bool ADZ_BaseCharacter::IsAlive() const
{
	return !bIsDead;
}

void ADZ_BaseCharacter::Death()
{
	if (!HasAuthority()) return;
	if (bIsDead) return;
	bIsDead = true;
	ClientHandleDeath();
}

void ADZ_BaseCharacter::ClientHandleDeath()
{
	OnDeathEvent.Broadcast();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void ADZ_BaseCharacter::BindDeathHandle()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (const UDZ_AttributeSet* AttributeSet = Cast<UDZ_AttributeSet>(ASC->GetAttributeSet(UDZ_AttributeSet::StaticClass())))
		{
			const_cast<UDZ_AttributeSet*>(AttributeSet)->OnDeath.AddDynamic(this, &ADZ_BaseCharacter::Death);
		}
	}
}

void ADZ_BaseCharacter::GiveStartupAbilities()
{
	UDZ_GameInstance* GameInstance = Cast<UDZ_GameInstance>(GetGameInstance());
	for (const FStartupAbilityInfo& Info : GameInstance->StartupAbilities->StartupAbilityInfos)
	{
		if (GetClass() != Info.CharacterClass) continue;
		
		for (const TSubclassOf<UGameplayAbility>& Ability : Info.Abilities)
		{
			GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability));
		}
		for (const TSubclassOf<UGameplayAbility>& Ability : Info.ActiveOnGivenAbilities)
		{
			GetAbilitySystemComponent()->GiveAbility(FGameplayAbilitySpec(Ability));
			GetAbilitySystemComponent()->TryActivateAbilityByClass(Ability);
		}
		return;
	}
}

void ADZ_BaseCharacter::OnRep_IsDead()
{
	ClientHandleDeath();
}
