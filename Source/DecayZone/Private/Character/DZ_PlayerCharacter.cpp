// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DZ_PlayerCharacter.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/DZ_InventoryComponent.h"
#include "DecayZone/DecayZone.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayEffect.h"
#include "GameplayTags/DZGameplayTags.h"
#include "Library/DZ_BlueprintLibrary.h"
#include "Player/DZ_PlayerState.h"

void ADZ_PlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	HighlightInteractable();
}

ADZ_PlayerCharacter::ADZ_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->bUsePawnControlRotation = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;
	
	InteractionBox = CreateDefaultSubobject<UBoxComponent>("InteractionBox");
	InteractionBox->SetupAttachment(GetRootComponent());
	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(Interactable, ECR_Overlap);
	InteractionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnInteractableBeginOverlap);
	InteractionBox->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnInteractableEndOverlap);
	
	InventoryComponent = CreateDefaultSubobject<UDZ_InventoryComponent>("InventoryComponent");
}

bool ADZ_PlayerCharacter::IsInteractableInRange(AActor* Actor)
{
	return OverlappingInteractables.Find(Actor) != nullptr;
}

void ADZ_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (InventoryComponent)
	{
		InventoryComponent->OnEquipmentsChanged.AddDynamic(this, &ThisClass::HandleEquipmentsChanged);
	}
}

void ADZ_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADZ_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ADZ_PlayerState* DZ_PlayerState = Cast<ADZ_PlayerState>(GetPlayerState());
	AbilitySystemComponent = Cast<UDZ_AbilitySystemComponent>(DZ_PlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(DZ_PlayerState, this);
	const FGameplayEffectSpecHandle& SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(InitializeGEClass, 1, AbilitySystemComponent->MakeEffectContext());
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
	const FGameplayEffectSpecHandle& EnergyRegenSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EnergyRegenGEClass, 1, AbilitySystemComponent->MakeEffectContext());
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EnergyRegenSpecHandle.Data);
	OnAttributesInitialized.Broadcast();
	GiveStartupAbilities();
	RecalculateArmor();
	BindDeathHandle();
}

void ADZ_PlayerCharacter::RecalculateArmor()
{
	if (!HasAuthority()) return;
	if (!AbilitySystemComponent) return;
	
	if (ArmorGEHandle.IsValid())
	{
		AbilitySystemComponent->RemoveActiveGameplayEffect(ArmorGEHandle);
	}
	
	if (!EquipmentArmorGEClass || !InventoryComponent) return;
	const float TotalArmor = InventoryComponent->GetTotalArmor();
	if (TotalArmor <= 0.f) return;
	
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EquipmentArmorGEClass, 1, Context);
	if (!SpecHandle.IsValid()) return;
	SpecHandle.Data->SetSetByCallerMagnitude(DZGameplayTags::SetByCaller_Armor, TotalArmor);
	ArmorGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
}

void ADZ_PlayerCharacter::HandleEquipmentsChanged()
{
	RecalculateArmor();
}

UAbilitySystemComponent* ADZ_PlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ADZ_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ADZ_PlayerState* DZ_PlayerState = Cast<ADZ_PlayerState>(GetPlayerState());
	AbilitySystemComponent = Cast<UDZ_AbilitySystemComponent>(DZ_PlayerState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(DZ_PlayerState, this);
}

void ADZ_PlayerCharacter::OnInteractableBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OverlappingInteractables.Add(OtherActor);
}

void ADZ_PlayerCharacter::OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingInteractables.Remove(OtherActor);
}

AActor* ADZ_PlayerCharacter::GetHighlightActor()
{
	return CurrentInteractableActor.Get();
}

void ADZ_PlayerCharacter::HighlightInteractable()
{
	if (!IsLocallyControlled()) return;
	AActor* TargetActor = FindTargetInteractableActor();
	if (TargetActor != CurrentInteractableActor)
	{
		OnFocusInteractableActorChanged.Broadcast(TargetActor);
		UDZ_BlueprintLibrary::SetActorHighlight(TargetActor, true);
		UDZ_BlueprintLibrary::SetActorHighlight(CurrentInteractableActor.Get(), false);
		CurrentInteractableActor = TargetActor;
	}
}

AActor* ADZ_PlayerCharacter::FindTargetInteractableActor()
{
	float MaxDot = -1;
	AActor* HighlightActor = nullptr;
	for (AActor* Actor : OverlappingInteractables)
	{
		if (!IsValid(Actor)) continue;
		const FVector ToInteractable = Actor->GetActorLocation() - GetActorLocation();
		float Dot = FVector::DotProduct(GetActorForwardVector(), ToInteractable.GetSafeNormal());
		if (Dot > MaxDot)
		{
			HighlightActor = Actor;
			MaxDot = Dot;
		}
	}
	return HighlightActor;
}