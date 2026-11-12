// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/DZ_AbilitySystemComponent.h"
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
#include "GameplayTags/DZGameplayTags.h"
#include "Interface/DZ_Interactable.h"
#include "Library/ItemHelper.h"
#include "Library/UIHelper.h"
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
	EnhancedInputComponent->BindAction(InputConfig->KickAction, ETriggerEvent::Started, this, &ADZ_PlayerController::Kick);
	EnhancedInputComponent->BindAction(InputConfig->WeaponAttackAction, ETriggerEvent::Started, this, &ADZ_PlayerController::WeaponAttack);
	
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
	LootPanelWidget->OnTakeItemRequested.AddDynamic(this, &ADZ_PlayerController::HandleTakeItemRequested);
	StatusBarsWidget->Open();
}
void ADZ_PlayerController::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	RefreshUIAll(false);
}

void ADZ_PlayerController::RefreshStatusBars()
{
	const float Energy = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetEnergyAttribute());
	const float MaxEnergy = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxEnergyAttribute());
	const float Health = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetHealthAttribute());
	const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxHealthAttribute());
	StatusBarsWidget->SetHealthInfo(Health, MaxHealth);
	StatusBarsWidget->SetEnergyInfo(Energy, MaxEnergy);
}

void ADZ_PlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	AbilitySystemComponent = Cast<UDZ_AbilitySystemComponent>(GetPlayerState<ADZ_PlayerState>()->GetAbilitySystemComponent());
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetEnergyAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetMaxEnergyAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);;
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);;
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);;
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UDZ_AttributeSet::GetArmorAttribute()).AddUObject(this, &ThisClass::OnAttributeValueChanged);
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
	if (!IsLocalController()) return;
	if (bIsInteracting)
	{
		bIsInteracting = false;
		ElapsedTime = 0.f;
		InteractProgressWidget->Close();
		InteractPromptWidget->SetVisibility(ESlateVisibility::Visible);
		ServerCancelInteract();
		return;
	}
	
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	if (!PlayerCharacter) return;
	AActor* InteractActor = PlayerCharacter->GetHighlightActor();
	if (!InteractActor) return;
	if (InteractActor->Implements<UDZ_Interactable>() && IDZ_Interactable::Execute_CanInteract(InteractActor))
	{
		TotalDuration = IDZ_Interactable::Execute_GetInteractDuration(InteractActor);
		ElapsedTime = 0;
		if (TotalDuration > 0.f)
		{
			InteractProgressWidget->Open();
		}
		bIsInteracting = true;
		InteractPromptWidget->Hide();
		ServerBeginInteract(InteractActor);
	}
}

void ADZ_PlayerController::UpdateInteractProgress()
{
	if (!IsLocalController()) return;
	if (TotalDuration == 0.f)
	{
		InteractProgressWidget->SetProgress(0, 0);
		return;
	}
	InteractProgressWidget->SetProgress(FMath::Clamp(ElapsedTime / TotalDuration, 0, 1), TotalDuration - ElapsedTime);
}

void ADZ_PlayerController::WeaponAttack()
{
	if (bIsInteracting) return;
	ActivateAbilityByTag(DZGameplayTags::Ability_Player_Attack_Weapon);
}

void ADZ_PlayerController::Kick()
{
	if (bIsInteracting) return;
	ActivateAbilityByTag(DZGameplayTags::Ability_Player_Attack_Kick);
}

void ADZ_PlayerController::ServerBeginInteract_Implementation(AActor* ContainerActor)
{
	if (!IsValid(ContainerActor)) return;
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	if (!IsValid(PlayerCharacter)) return;
	if (!Cast<IDZ_Interactable>(ContainerActor)) return;

	FGameplayEventData Payload;
	Payload.Target = ContainerActor;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), DZGameplayTags::Event_Trigger_Interact, Payload);
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
	if (!IsLocalController()) return;
	RefreshStatusBars();
	
	UDZ_InventoryComponent* InventoryComp = GetPawn()->FindComponentByClass<UDZ_InventoryComponent>();
	if (!InventoryComp) return;
	
	FDZ_CharacterStatsInfo StatsInfo;
	StatsInfo.Weight = FMath::RoundToInt(InventoryComp->GetTotalWeight());
	StatsInfo.MaxWeight = FMath::RoundToInt(InventoryComp->GetMaxWeight());
	if (AbilitySystemComponent.IsValid())
	{
		StatsInfo.Health = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetHealthAttribute()));
		StatsInfo.MaxHealth = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxHealthAttribute()));
		StatsInfo.Energy = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetEnergyAttribute()));
		StatsInfo.MaxEnergy = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxEnergyAttribute()));
		StatsInfo.Armor = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetArmorAttribute()));
	}
	
	if (bForceRefresh || IsValid(InventoryPanelWidget) && InventoryPanelWidget->IsInViewport())
	{
		InventoryPanelWidget->SetBackpackPanel(InventoryComp->GetItems());
		InventoryPanelWidget->SetEquipmentPanel(InventoryComp->GetEquipments());
		InventoryPanelWidget->SetCharacterStatsInfo(StatsInfo);
		InventoryPanelWidget->SetWeightInfo(InventoryComp->GetTotalWeight(), InventoryComp->GetMaxWeight());
	}
	if (bForceRefresh || IsValid(LootPanelWidget) && LootPanelWidget->IsInViewport())
	{
		LootPanelWidget->SetBackpackPanel(InventoryComp->GetItems());
		LootPanelWidget->SetEquipmentPanel(InventoryComp->GetEquipments());
		LootPanelWidget->SetCharacterStatsInfo(StatsInfo);
		LootPanelWidget->SetWeightInfo(InventoryComp->GetTotalWeight(), InventoryComp->GetMaxWeight());
		if (InteractContainerComp.IsValid())
		{
			LootPanelWidget->SetLootListInfo(InteractContainerComp->GetItems());
		}
	}
}

void ADZ_PlayerController::NotifyInteractFinished(AActor* Target, bool bSucceeded)
{
	ClientInteractFinished(Target, bSucceeded);
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

void ADZ_PlayerController::ServerCancelInteract_Implementation()
{
	UDZ_AbilitySystemComponent* ASC = Cast<UDZ_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	if (!ASC) return;


	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecByTag(DZGameplayTags::Ability_Interact);
	if (!Spec) return;

	ASC->CancelAbilityHandle(Spec->Handle);
}

void ADZ_PlayerController::ServerTakeItem_Implementation(AActor* ContainerActor, int32 SlotIndex, int32 Count)
{
	if (!IsValid(ContainerActor)) return;

	UDZ_ContainerComponent* Container = ContainerActor->FindComponentByClass<UDZ_ContainerComponent>();
	if (!Container) return;
	
	APawn* LocalPawn = GetPawn();
	if (!LocalPawn) return;
	UDZ_InventoryComponent* InventoryComp = LocalPawn->FindComponentByClass<UDZ_InventoryComponent>();
	if (!InventoryComp) return;
	
	if (ItemHelper::TakeItem(InventoryComp, Container, SlotIndex, Count))
	{
		RefreshUIAll(false);
	}
}

void ADZ_PlayerController::HandleTakeItemRequested(UWidget* Row)
{
	UDZ_ItemSlotBase* RowWidget = Cast<UDZ_ItemSlotBase>(Row);
	if (!RowWidget) return;
	if (!InteractTarget.IsValid()) return;
	
	const FDZ_ItemSlotHandle Handle = RowWidget->GetSlotHandle();
	ServerTakeItem(InteractTarget.Get(), Handle.SlotIndex, 1);
}

bool ADZ_PlayerController::IsMovementLocked() const
{
	if (bIsInteracting) return true;
	if (!AbilitySystemComponent.IsValid()) return false;
	if (AbilitySystemComponent->HasMatchingGameplayTag(DZGameplayTags::State_MovementLocked) || 
		AbilitySystemComponent->HasMatchingGameplayTag(DZGameplayTags::State_Interacting))
	{
		return true;
	}
	return false;
}

void ADZ_PlayerController::Move(const FInputActionValue& Value)
{
	if (IsMovementLocked()) return;
	if (!GetPawn()) return;
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
	if (IsMovementLocked()) return;
	if (!GetCharacter()) return;
	
	GetCharacter()->Jump();
	UIHelper::ShowTips(this, FText::FromString("Jumping"), 3);
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
		RefreshUIAll(false);
	}
}
