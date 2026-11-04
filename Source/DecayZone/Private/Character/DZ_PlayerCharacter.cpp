// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/DZ_PlayerCharacter.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "DecayZone/DecayZone.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/SpringArmComponent.h"
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
}

void ADZ_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
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
	GiveStartupAbilities();
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
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("AAAAAAAAAAAAAAAAAA")));
}

void ADZ_PlayerCharacter::OnInteractableEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingInteractables.Remove(OtherActor);
}

void ADZ_PlayerCharacter::HighlightInteractable()
{
	float MaxDot = -1;
	AActor* HighlightActor = nullptr;
	for (AActor* Actor : OverlappingInteractables)
	{
		const FVector ToInteractable = Actor->GetActorLocation() - GetActorLocation();
		float Dot = FVector::DotProduct(GetActorForwardVector(), ToInteractable.GetSafeNormal());
		if (Dot > MaxDot)
		{
			HighlightActor = Actor;
			MaxDot = Dot;
		}
	}
	if (CurrentInterctableActor.IsValid())
	{
		UDZ_BlueprintLibrary::SetActorHighlight(CurrentInterctableActor.Get(), false);
	}
	UDZ_BlueprintLibrary::SetActorHighlight(HighlightActor, true);
	CurrentInterctableActor = HighlightActor;
}

