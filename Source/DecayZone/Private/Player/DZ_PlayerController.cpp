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
#include "Data/Ability/DZ_SkillDefines.h"
#include "Data/Input/DZ_InputConfig.h"
#include "Data/Item/FDZ_ItemData.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Data/UI/FDZ_SettlementInfo.h"
#include "DecayZone/DecayZone.h"
#include "Game/DZ_GameInstance.h"
#include "Game/DZ_GameState.h"
#include "GameFramework/Character.h"
#include "GameplayTags/DZGameplayTags.h"
#include "Interface/DZ_Interactable.h"
#include "Library/ItemHelper.h"
#include "Library/UIHelper.h"
#include "Player/DZ_PlayerState.h"
#include "UI/Controllers/DZ_AbilityBarController.h"
#include "UI/Controllers/DZ_InteractController.h"
#include "UI/Controllers/DZ_InventoryController.h"
#include "UI/Controllers/DZ_LootController.h"
#include "UI/Controllers/DZ_SettlementController.h"
#include "UI/Controllers/DZ_StatusBarsController.h"
#include "UI/UIManager.h"
#include "UI/Widgets/Inventory/DZ_EquipmentPanel.h"


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
	EnhancedInputComponent->BindAction(InputConfig->InteractAction, ETriggerEvent::Started, this, &ADZ_PlayerController::InteractHighlightActor);
	EnhancedInputComponent->BindAction(InputConfig->CloseTopWidgetAction, ETriggerEvent::Started, this, &ADZ_PlayerController::CloseTopWidget);
	EnhancedInputComponent->BindAction(InputConfig->KickAction, ETriggerEvent::Started, this, &ADZ_PlayerController::Kick);
	EnhancedInputComponent->BindAction(InputConfig->WeaponAttackAction, ETriggerEvent::Started, this, &ADZ_PlayerController::WeaponAttack);
	
}

void ADZ_PlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UIManager = NewObject<UUIManager>(this);
}

UUIManager* ADZ_PlayerController::GetUIManager() const
{
	return UIManager;
}

void ADZ_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (!IsLocalController()) return;
	if (!IsValid(UIManager)) return;
	
	UIManager->Initialize(this);
}

void ADZ_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(UIManager))
	{
		UIManager->Shutdown();
	}
	UIManager = nullptr;
	
	Super::EndPlay(EndPlayReason);
}

void ADZ_PlayerController::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	RefreshUIAll(false);
}

void ADZ_PlayerController::RefreshStatusBars()
{
	if (!IsValid(UIManager)) return;
	
	UIManager->StatusBarsController->Refresh();
}

void ADZ_PlayerController::SetSlotSkill(int32 SlotIndex, int32 SkillId)
{
	if (!IsValid(UIManager)) return;
	
	UIManager->AbilityBarController->SetSlotSkill(SlotIndex, SkillId);
}

int32 ADZ_PlayerController::GetSlotSkill(int32 SlotIndex) const
{
	if (!IsValid(UIManager)) return DZSkill::EmptyId;
	
	return UIManager->AbilityBarController->GetSlotSkill(SlotIndex);
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
	if (!IsLocalController()) return;
	if (!IsValid(UIManager)) return;
	
	UIManager->TickControllers(DeltaSeconds);
}

void ADZ_PlayerController::InteractHighlightActor()
{
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	Interact(PlayerCharacter ? PlayerCharacter->GetHighlightActor() : nullptr);
}

void ADZ_PlayerController::BeginInteract(AActor* Target)
{
	if (!IsLocalController()) return;
	if (!IsValid(UIManager)) return;
	
	UIManager->InteractController->BeginInteract(Target);
}

void ADZ_PlayerController::CancelInteract()
{
	if (!IsLocalController()) return;
	if (!IsValid(UIManager)) return;
	
	UIManager->InteractController->CancelInteract();
}

void ADZ_PlayerController::EndInteract()
{
	if (!IsValid(UIManager)) return;
	
	UIManager->InteractController->EndInteract();
}

void ADZ_PlayerController::Interact(AActor* InteractActor)
{
	if (!IsLocalController()) return;
	if (!IsValid(UIManager)) return;
	
	UIManager->InteractController->Interact(InteractActor);
}

void ADZ_PlayerController::WeaponAttack()
{
	if (IsValid(UIManager) && UIManager->InteractController->IsBlockingAttack()) return;
	
	ActivateAbilityByTag(DZGameplayTags::Ability_Player_Attack_Weapon);
}

void ADZ_PlayerController::Kick()
{
	if (IsValid(UIManager) && UIManager->InteractController->IsBlockingAttack()) return;
	
	ActivateAbilityByTag(DZGameplayTags::Ability_Player_Attack_Kick);
}

void ADZ_PlayerController::ServerBeginInteract_Implementation(AActor* ContainerActor)
{
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	UDZ_AbilitySystemComponent* ASC = Cast<UDZ_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	if (!ASC) return;
	if (!IsValid(PlayerCharacter)) return;
	if (!Cast<IDZ_Interactable>(ContainerActor)) return;
	FGameplayEventData Payload;
	Payload.Target = ContainerActor;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), DZGameplayTags::Event_Trigger_Interact, Payload);
		
	const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecByTag(DZGameplayTags::Ability_Interact);
	if (!Spec || !Spec->IsActive())
	{
		ClientInteractFinished(ContainerActor, false);
	}
}

void ADZ_PlayerController::ClientInteractFinished_Implementation(AActor* Target, bool bSucceeded)
{
	if (!IsValid(UIManager)) return;
	
	UIManager->InteractController->EndInteract();
}

void ADZ_PlayerController::ClientOpenLootPanel_Implementation(UDZ_ContainerComponent* Container)
{
	if (!IsValid(Container)) return;
	if (!IsValid(UIManager)) return;

	if (InteractContainerComp.IsValid())
	{
		InteractContainerComp->OnContentsChanged.RemoveAll(this);
	}

	InteractContainerComp = Container;
	InteractContainerComp->OnContentsChanged.AddDynamic(this, &ThisClass::HandleContainerItemsChanged);
	UIManager->LootController->Open();
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

void ADZ_PlayerController::ClientOpenSettlementPanel_Implementation(const FDZ_SettlementInfo& SettlementInfo)
{
	if (!IsValid(UIManager)) return;
	
	UIManager->AbilityBarController->Hide();
	UIManager->SettlementController->Show(SettlementInfo);
}

void ADZ_PlayerController::NotifyGameEnd(bool bExtracted)
{
	if (!HasAuthority()) return;
	
	Settle(bExtracted ? EDZExtractResult::Extracted : EDZExtractResult::Dead);
}

void ADZ_PlayerController::Settle(EDZExtractResult Result)
{
	if (!HasAuthority()) return;
	
	ADZ_PlayerState* DZPlayerState = GetPlayerState<ADZ_PlayerState>();
	if (!DZPlayerState) return;
	if (DZPlayerState->GetExtractResult() != EDZExtractResult::InProgress) return;
	
	DZPlayerState->SetExtractResult(Result);
	ClientOpenSettlementPanel(BuildSettlementInfo());
}

void ADZ_PlayerController::RefreshUIAll(bool bForceRefresh)
{
	if (!IsLocalController()) return;
	RefreshStatusBars();
	
	if (!GetInventoryComponent()) return;
	
	if (!IsValid(UIManager)) return;
	
	UIManager->InventoryController->Refresh(bForceRefresh);
	UIManager->LootController->Refresh(bForceRefresh);
}

UDZ_InventoryComponent* ADZ_PlayerController::GetInventoryComponent() const
{
	APawn* OwnerPawn = GetPawn();
	return OwnerPawn ? OwnerPawn->FindComponentByClass<UDZ_InventoryComponent>() : nullptr;
}

bool ADZ_PlayerController::BuildCharacterStats(FDZ_CharacterStatsInfo& OutStats) const
{
	UDZ_InventoryComponent* InventoryComp = GetInventoryComponent();
	if (!InventoryComp) return false;
	
	OutStats.Weight = FMath::RoundToInt(InventoryComp->GetTotalWeight());
	OutStats.MaxWeight = FMath::RoundToInt(InventoryComp->GetMaxWeight());
	if (AbilitySystemComponent.IsValid())
	{
		OutStats.Health = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetHealthAttribute()));
		OutStats.MaxHealth = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxHealthAttribute()));
		OutStats.Energy = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetEnergyAttribute()));
		OutStats.MaxEnergy = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxEnergyAttribute()));
		OutStats.Armor = FMath::RoundToInt(AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetArmorAttribute()));
	}
	return true;
}

TWeakObjectPtr<UDZ_ContainerComponent> ADZ_PlayerController::GetInteractContainer() const
{
	return InteractContainerComp;
}

void ADZ_PlayerController::NotifyInteractFinished(AActor* Target, bool bSucceeded)
{
	ClientInteractFinished(Target, bSucceeded);
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
	if (!IsValid(UIManager)) return;
	
	UIManager->InteractController->OnFocusInteractableChanged(Actor);
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

bool ADZ_PlayerController::IsMovementLocked() const
{
	if (IsValid(UIManager) && UIManager->InteractController->IsBlockingAttack()) return true;
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
	if (!IsValid(UIManager)) return;
	
	UIManager->CloseTopWidget();
}

void ADZ_PlayerController::OpenOrCloseInventoryPanel()
{
	if (!IsValid(UIManager)) return;
	
	if (UIManager->InventoryController->Toggle())
	{
		RefreshUIAll(false);
	}
}

FDZ_SettlementInfo ADZ_PlayerController::BuildSettlementInfo() const
{
	FDZ_SettlementInfo Info;
	if (ADZ_PlayerState* DZPlayerState = GetPlayerState<ADZ_PlayerState>())
	{
		Info.bSucceeded = (DZPlayerState->GetExtractResult() == EDZExtractResult::Extracted);
		Info.EnemyKills = DZPlayerState->GetEnemyKills();
		Info.PlayerKills = DZPlayerState->GetPlayerKills();
	}
	
	if (ADZ_GameState* GameState = GetWorld()->GetGameState<ADZ_GameState>())
	{
		Info.ElapsedSeconds = GameState->GetElapsedTime();
	}
	
	APawn* PlayerPawn = GetPawn();
	if (UDZ_InventoryComponent* InventoryComp = PlayerPawn ? PlayerPawn->FindComponentByClass<UDZ_InventoryComponent>() : nullptr)
	{
		Info.Items = InventoryComp->GetAllItems();
		Info.TotalValue = InventoryComp->GetTotalValue();
	}
	return Info;
}
