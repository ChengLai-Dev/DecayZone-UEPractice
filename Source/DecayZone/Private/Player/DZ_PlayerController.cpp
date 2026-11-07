// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Blueprint/UserWidget.h"
#include "Character/DZ_PlayerCharacter.h"
#include "Components/DZ_ContainerComponent.h"
#include "Components/DZ_InventoryComponent.h"
#include "Data/Input/DZ_InputConfig.h"
#include "Data/UI/DZ_UIConfig.h"
#include "DecayZone/DecayZone.h"
#include "Game/DZ_GameInstance.h"
#include "GameFramework/Character.h"
#include "Interface/DZ_Interactable.h"
#include "Library/ItemHelper.h"
#include "Player/DZ_PlayerState.h"
#include "UI/UIManager.h"
#include "UI/Widgets/DZ_InteractProgress.h"
#include "UI/Widgets/DZ_InventoryPanel.h"
#include "UI/Widgets/DZ_InteractPrompt.h"
#include "UI/Widgets/DZ_ItemSlotBase.h"
#include "UI/Widgets/DZ_LootPanel.h"
#include "UI/Widgets/DZ_StatusBars.h"


void ADZ_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!IsValid(InputConfig))
	{
		UE_LOG(LogDZ, Warning, TEXT("no InputConfig found"));
		return;
	}
	UEnhancedInputLocalPlayerSubsystem* Subsystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	Subsystem->AddMappingContext(InputConfig->DefaultInputMappingContext, 0);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ADZ_PlayerController::Look);
	EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ADZ_PlayerController::Jump);
	EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ADZ_PlayerController::Move);
	EnhancedInputComponent->BindAction(InputConfig->OpenOrCloseInventoryAction, ETriggerEvent::Started, this, &ADZ_PlayerController::OpenOrCloseInventoryPanel);
	EnhancedInputComponent->BindAction(InputConfig->InteractAction, ETriggerEvent::Started, this, &ADZ_PlayerController::Interact);
	EnhancedInputComponent->BindAction(InputConfig->CloseTopWidgetAction, ETriggerEvent::Started, this, &ADZ_PlayerController::CloseTopWidget);
	
}

void ADZ_PlayerController::BeginPlay()
{
	if (!IsLocalController()) return;
	Super::BeginPlay();
	UDZ_UIConfig* UIConfig = Cast<UDZ_GameInstance>(GetGameInstance())->UIConfig;
	InventoryPanelWidget = CreateWidget<UDZ_InventoryPanel>(this, UIConfig->InventoryWidgetClass);
	InteractPromptWidget = CreateWidget<UDZ_InteractPrompt>(this, UIConfig->InteractPromptClass);
	InteractProgressWidget = CreateWidget<UDZ_InteractProgress>(this, UIConfig->InteractProgressClass);
	LootPanelWidget = CreateWidget<UDZ_LootPanel>(this, UIConfig->LootPanelClass);
	StatusBarsWidget = CreateWidget<UDZ_StatusBars>(this, UIConfig->StatusBarsClass);
	
	InventoryPanelWidget->OnItemMoveRequested.AddDynamic(this, &ADZ_PlayerController::HandleItemMoveRequested);
	LootPanelWidget->OnItemMoveRequested.AddDynamic(this, &ADZ_PlayerController::HandleItemMoveRequested);
	StatusBarsWidget->Open();
}
void ADZ_PlayerController::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	RefreshStatusBars();
}

void ADZ_PlayerController::RefreshStatusBars()
{
	float Enegy = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetEnergyAttribute());
	float MaxEnegy = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxEnergyAttribute());
	float Health = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetHealthAttribute());
	float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxHealthAttribute());
	StatusBarsWidget->SetHealthInfo(Health, MaxHealth);
	StatusBarsWidget->SetEnergyInfo(Enegy, MaxEnegy);
}

void ADZ_PlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AbilitySystemComponent = GetPlayerState<ADZ_PlayerState>()->GetAbilitySystemComponent();
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetEnergyAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetMaxEnergyAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);;
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);;
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);;
	}
}

void ADZ_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsInteracting && IsLocalController())
	{
		ElapsedTime += DeltaSeconds;
		UpdateInteractProgress();
	}
}

void ADZ_PlayerController::Interact()
{
	// ActivateAbilityByTag(DZGameplayTags::Ability_Interact);
	if (!IsLocalController()) return;
	
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	AActor* InteractActor = PlayerCharacter->GetHighlightActor();
	if (Cast<IDZ_Interactable>(InteractActor))
	{
		TotalDuration = IDZ_Interactable::Execute_GetInteractDuration(InteractActor);
		ElapsedTime = 0;
		if (TotalDuration == 0.f)
		{
			InteractProgressWidget->Close();
			return;
		}
		InteractProgressWidget->Open();
		bIsInteracting = true;
		InteractPromptWidget->Hide();
		ServerBeginInteract(InteractActor);
	}
}

void ADZ_PlayerController::UpdateInteractProgress()
{
	if (!IsLocalController()) return;
	InteractProgressWidget->SetProgress(FMath::Clamp(ElapsedTime / TotalDuration, 0, 1), TotalDuration - ElapsedTime);
}

void ADZ_PlayerController::ServerBeginInteract_Implementation(AActor* ContainerActor)
{
	if (!IsValid(ContainerActor)) return;
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	if (!IsValid(PlayerCharacter)) return;
	if (!Cast<IDZ_Interactable>(ContainerActor)) return;
	
	float Duration = IDZ_Interactable::Execute_GetInteractDuration(ContainerActor);
	GetWorldTimerManager().ClearTimer(InteractTimerHandle);
	InteractTarget = ContainerActor;
	
	if (Duration <= 0.f)
	{
		FinishInteract();
		return;
	}
	GetWorldTimerManager().SetTimer(InteractTimerHandle, this, &ADZ_PlayerController::FinishInteract, Duration, false);
}

void ADZ_PlayerController::ClientInteractFinished_Implementation(AActor* Target, bool bSucceeded)
{
	InteractProgressWidget->Close();
	InteractPromptWidget->SetVisibility(ESlateVisibility::Visible);
	bIsInteracting = false;
	if (!bSucceeded) return;
	
	if (InteractContainerComp.IsValid())
	{
		InteractContainerComp->OnContentsChanged.RemoveAll(this);
	}
	InteractContainerComp = Target->FindComponentByClass<UDZ_ContainerComponent>();
	if (!InteractContainerComp.IsValid()) return;
	InteractContainerComp->OnContentsChanged.AddDynamic(this, &ThisClass::HandleContainerItemsChanged);
	LootPanelWidget->Open();
	RefreshUIAll(false);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
}

void ADZ_PlayerController::ServerMoveItem_Implementation(UDZ_InventoryComponent* InventoryComp,
	UDZ_ContainerComponent* ContainerComp, const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To)
{
	if (ItemHelper::MoveItem(InventoryComp, ContainerComp, From, To))
	{
		RefreshUIAll(false);
	}
}


void ADZ_PlayerController::RefreshUIAll(bool bForceRefresh)
{
	UDZ_InventoryComponent* InventoryComp = GetPawn()->FindComponentByClass<UDZ_InventoryComponent>();
	if (bForceRefresh || IsValid(InventoryPanelWidget) && InventoryPanelWidget->IsInViewport())
	{
		InventoryPanelWidget->SetBackpackPanel(InventoryComp->GetItems());
	}
	if (bForceRefresh || IsValid(LootPanelWidget) && LootPanelWidget->IsInViewport())
	{
		LootPanelWidget->SetBackpackPanel(InventoryComp->GetItems());
		if (InteractContainerComp.IsValid())
		{
			LootPanelWidget->SetLootListInfo(InteractContainerComp->GetItems());
		}
	}
	RefreshStatusBars();
}


void ADZ_PlayerController::HandleItemMoveRequested(UWidget* From, UWidget* To)
{
	UDZ_ItemSlotBase* FromWidget = Cast<UDZ_ItemSlotBase>(From);
	UDZ_ItemSlotBase* ToWidget   = Cast<UDZ_ItemSlotBase>(To);
	if (!FromWidget || !ToWidget) return;
	
	UDZ_InventoryComponent* InventoryComp = GetPawn()->FindComponentByClass<UDZ_InventoryComponent>();

	ServerMoveItem(InventoryComp, InteractContainerComp.Get(), FromWidget->GetSlotHandle(), ToWidget->GetSlotHandle());
}

void ADZ_PlayerController::HandleInventoryItemsChanged()
{
	RefreshUIAll(false);
}

void ADZ_PlayerController::HandleContainerItemsChanged()
{
	RefreshUIAll(false);
}

void ADZ_PlayerController::FinishInteract()
{
	AActor* Target = InteractTarget.Get();
	InteractTarget = nullptr;
	
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	if (!IsValid(Target) || !PlayerCharacter || !PlayerCharacter->IsInteractableInRange(Target))
	{
		ClientInteractFinished(Target, false);
	}
	else
	{
		ClientInteractFinished(Target, true);
	}
	
}

void ADZ_PlayerController::ActivateAbilityByTag(const FGameplayTag& AbilityTag)
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

void ADZ_PlayerController::HandleInteractableActorChanged(AActor* Actor)
{
	if (!IsLocalController()) return;
	
	if (!IsValid(Actor))
	{
		InteractPromptWidget->Close();
		return;
	}
	
	if (Cast<IDZ_Interactable>(Actor))
	{
		InteractPromptWidget->Open();
		FText InteractPrompt = IDZ_Interactable::Execute_GetInteractPromptText(Actor);
		FText InteractKey; 
		for (const auto& Mapping: InputConfig->DefaultInputMappingContext->GetMappings())
		{
			if (Mapping.Action == InputConfig->InteractAction)
			{
				InteractKey = Mapping.Key.GetDisplayName();
				break;
			}
		}
		InteractPromptWidget->SetKeyText(InteractKey);
		InteractPromptWidget->SetPromptText(InteractPrompt);
	}
}

void ADZ_PlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	if (ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(InPawn))
	{
		PlayerCharacter->OnFocusInteractableActorChanged.AddDynamic(this, &ADZ_PlayerController::HandleInteractableActorChanged);
		if (UDZ_InventoryComponent* InventoryComp = PlayerCharacter->FindComponentByClass<UDZ_InventoryComponent>())
		{
			InventoryComp->OnItemsChanged.AddDynamic(this, &ADZ_PlayerController::HandleInventoryItemsChanged);
		}
		PlayerCharacter->OnAttributesInitialized.AddDynamic(this, &ADZ_PlayerController::RefreshStatusBars);
	}
}

void ADZ_PlayerController::ServerTakeItem_Implementation(AActor* ContainerActor, int32 SlotIndex, int32 Count)
{
	if (!IsValid(ContainerActor)) return;

	UDZ_ContainerComponent* Container = ContainerActor->FindComponentByClass<UDZ_ContainerComponent>();
	if (!Container) return;

	Container->TryTake(SlotIndex, Count);
}

void ADZ_PlayerController::Move(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	const FRotator YawRotation = FRotator(0, GetControlRotation().Yaw, 0);
	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	GetPawn()->AddMovementInput(ForwardVector, InputVector.Y);
	GetPawn()->AddMovementInput(RightVector, InputVector.X);
}

void ADZ_PlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	AddYawInput(InputVector.X);
	AddPitchInput(InputVector.Y);
}

void ADZ_PlayerController::Jump(const FInputActionValue& Value)
{
	GetCharacter()->Jump();
}

void ADZ_PlayerController::CloseTopWidget()
{
	UUIManager* UIManager = ULocalPlayer::GetSubsystem<UUIManager>(GetLocalPlayer());
	UIManager->CloseTopWidget();
}

void ADZ_PlayerController::OpenOrCloseInventoryPanel()
{
	if (!InventoryPanelWidget) return;
	
	if (InventoryPanelWidget->IsInViewport())
	{
		InventoryPanelWidget->Close();
	}
	else
	{
		InventoryPanelWidget->Open();
		UDZ_InventoryComponent* InventoryComp = GetPawn()->FindComponentByClass<UDZ_InventoryComponent>();
		if (InventoryComp)
		{
			InventoryPanelWidget->SetBackpackPanel(InventoryComp->GetItems());
		}
	}
}
