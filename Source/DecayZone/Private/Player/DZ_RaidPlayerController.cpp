// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DZ_RaidPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "Character/DZ_PlayerCharacter.h"
#include "Components/DZ_ContainerComponent.h"
#include "Components/DZ_InventoryComponent.h"
#include "Data/Ability/DZ_SkillDefines.h"
#include "Data/Ability/FDZ_SkillData.h"
#include "Data/Input/DZ_InputConfig.h"
#include "Data/Item/DZ_ItemDefines.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "Data/Item/FDZ_Loadout.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Data/UI/FDZ_SettlementInfo.h"
#include "DecayZone/DecayZone.h"
#include "Game/DZ_GameInstance.h"
#include "Game/DZ_GameState.h"
#include "GameFramework/Character.h"
#include "GameplayTags/DZGameplayTags.h"
#include "Interface/DZ_Interactable.h"
#include "Kismet/GameplayStatics.h"
#include "Library/ItemHelper.h"
#include "Player/DZ_PlayerState.h"
#include "UI/Controllers/DZ_HUDController.h"
#include "UI/Controllers/DZ_InteractController.h"
#include "UI/Controllers/DZ_InventoryController.h"
#include "UI/Controllers/DZ_LootController.h"
#include "UI/Controllers/DZ_SettlementController.h"
#include "UI/DZ_RaidUIManager.h"
#include "UI/UIManager.h"
#include "UI/Widgets/HUD/DZ_Tips.h"
#include "UI/Widgets/Inventory/DZ_EquipmentPanel.h"


ADZ_RaidPlayerController::ADZ_RaidPlayerController()
{
	InventoryComponent = CreateDefaultSubobject<UDZ_InventoryComponent>("InventoryComponent");
}

void ADZ_RaidPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->OnItemsChanged.AddUniqueDynamic(this, &ThisClass::HandleInventoryItemsChanged);
	}
}

void ADZ_RaidPlayerController::SetupInputComponent()
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
	EnhancedInputComponent->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ADZ_RaidPlayerController::Look);
	EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::Jump);
	EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ADZ_RaidPlayerController::Move);
	EnhancedInputComponent->BindAction(InputConfig->OpenOrCloseInventoryAction, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::OpenOrCloseInventoryPanel);
	EnhancedInputComponent->BindAction(InputConfig->InteractAction, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::InteractHighlightActor);
	EnhancedInputComponent->BindAction(InputConfig->CloseTopWidgetAction, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::CloseTopWidget);
	EnhancedInputComponent->BindAction(InputConfig->KickAction, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::Kick);
	EnhancedInputComponent->BindAction(InputConfig->WeaponAttackAction, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::WeaponAttack);
	EnhancedInputComponent->BindAction(InputConfig->Skill1, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::Skill1);
	EnhancedInputComponent->BindAction(InputConfig->Skill2, ETriggerEvent::Started, this, &ADZ_RaidPlayerController::Skill2);
	
}

UUIManager* ADZ_RaidPlayerController::CreateUIManager()
{
	RaidUI = NewObject<UDZ_RaidUIManager>(this);
	return RaidUI;
}

void ADZ_RaidPlayerController::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	RefreshUIAll(false);
}

void ADZ_RaidPlayerController::RefreshStatusBars()
{
	if (!IsValid(RaidUI)) return;
	
	RaidUI->HUDController->RefreshStatusBars();
}

void ADZ_RaidPlayerController::SetSlotSkill(int32 SlotIndex, int32 SkillId)
{
	UDZ_GameInstance* GameInstance = Cast<UDZ_GameInstance>(GetGameInstance());
	if (!GameInstance) return;
	
	GameInstance->SetSlotSkillId(SlotIndex, SkillId);
	
	if (IsValid(RaidUI))
	{
		RaidUI->HUDController->RefreshAbilityBar();
	}
}

int32 ADZ_RaidPlayerController::GetSlotSkill(int32 SlotIndex) const
{
	const UDZ_GameInstance* GameInstance = Cast<UDZ_GameInstance>(GetGameInstance());
	if (!GameInstance) return DZSkill::EmptyId;
	
	return GameInstance->GetSlotSkillId(SlotIndex);
}

void ADZ_RaidPlayerController::OnRep_PlayerState()
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

void ADZ_RaidPlayerController::InteractHighlightActor()
{
	ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(GetPawn());
	Interact(PlayerCharacter ? PlayerCharacter->GetHighlightActor() : nullptr);
}

void ADZ_RaidPlayerController::BeginInteract(AActor* Target)
{
	if (!IsLocalController()) return;
	if (!IsValid(RaidUI)) return;
	
	RaidUI->InteractController->BeginInteract(Target);
}

void ADZ_RaidPlayerController::CancelInteract()
{
	if (!IsLocalController()) return;
	if (!IsValid(RaidUI)) return;
	
	RaidUI->InteractController->CancelInteract();
}

void ADZ_RaidPlayerController::EndInteract()
{
	if (!IsValid(RaidUI)) return;
	
	RaidUI->InteractController->EndInteract();
}

void ADZ_RaidPlayerController::Interact(AActor* InteractActor)
{
	if (!IsLocalController()) return;
	if (!IsValid(RaidUI)) return;
	
	RaidUI->InteractController->Interact(InteractActor);
}

void ADZ_RaidPlayerController::WeaponAttack()
{
	if (IsValid(RaidUI) && RaidUI->InteractController->IsBlockingAction()) return;
	
	ActivateAbilityByTag(DZGameplayTags::Ability_Player_Attack_Weapon);
}

void ADZ_RaidPlayerController::Kick()
{
	if (IsValid(RaidUI) && RaidUI->InteractController->IsBlockingAction()) return;
	
	ActivateAbilityByTag(DZGameplayTags::Ability_Player_Attack_Kick);
}

void ADZ_RaidPlayerController::ServerBeginInteract_Implementation(AActor* ContainerActor)
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

void ADZ_RaidPlayerController::ClientInteractFinished_Implementation(AActor* Target, bool bSucceeded)
{
	if (!IsValid(RaidUI)) return;
	
	RaidUI->InteractController->EndInteract();
}

void ADZ_RaidPlayerController::ClientOpenLootPanel_Implementation(UDZ_ContainerComponent* Container)
{
	if (!IsValid(Container)) return;
	if (!IsValid(RaidUI)) return;

	if (InteractContainerComp.IsValid())
	{
		InteractContainerComp->OnContentsChanged.RemoveAll(this);
	}

	InteractContainerComp = Container;
	InteractContainerComp->OnContentsChanged.AddDynamic(this, &ThisClass::HandleContainerItemsChanged);
	RaidUI->LootController->Open();
	RefreshUIAll(false);
}

void ADZ_RaidPlayerController::ServerMoveItem_Implementation(UDZ_ContainerComponent* ContainerComp,
	const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To)
{
	UDZ_InventoryComponent* InventoryComp = GetInventoryComponent();
	if (!InventoryComp) return;
	
	if (ItemHelper::MoveItem(InventoryComp, ContainerComp, From, To))
	{
		RefreshUIAll(false);
	}
}

void ADZ_RaidPlayerController::ClientOpenSettlementPanel_Implementation(const FDZ_SettlementInfo& SettlementInfo)
{
	if (!IsValid(RaidUI)) return;
	
	RaidUI->HUDController->HideAbilityBar();
	RaidUI->SettlementController->Show(SettlementInfo);
}

void ADZ_RaidPlayerController::NotifyGameEnd(bool bExtracted)
{
	if (!HasAuthority()) return;
	
	if (!Settle(bExtracted ? EDZExtractResult::Extracted : EDZExtractResult::Dead)) return;
	
	if (bExtracted) return;
	
	APawn* OwnPawn = GetPawn();
	UDZ_ContainerComponent* ContainerComp = OwnPawn ? OwnPawn->FindComponentByClass<UDZ_ContainerComponent>() : nullptr;
	if (!ContainerComp)
	{
		UE_LOG(LogDZ, Warning, TEXT("NotifyGameEnd: no container component found on pawn, corpse will be empty"));
		return;
	}
	
	TransferInventoryToContainer(ContainerComp);
}

bool ADZ_RaidPlayerController::Settle(EDZExtractResult Result)
{
	if (!HasAuthority()) return false;
	
	ADZ_PlayerState* DZPlayerState = GetPlayerState<ADZ_PlayerState>();
	if (!DZPlayerState) return false;
	if (DZPlayerState->GetExtractResult() != EDZExtractResult::InProgress) return false;
	
	DZPlayerState->SetExtractResult(Result);
	ClientOpenSettlementPanel(BuildSettlementInfo());
	return true;
}

void ADZ_RaidPlayerController::RefreshUIAll(bool bForceRefresh)
{
	if (!IsLocalController()) return;
	RefreshStatusBars();
	
	if (!GetInventoryComponent()) return;
	
	if (!IsValid(RaidUI)) return;
	
	RaidUI->InventoryController->Refresh(bForceRefresh);
	RaidUI->LootController->Refresh(bForceRefresh);
}

UDZ_InventoryComponent* ADZ_RaidPlayerController::GetInventoryComponent() const
{
	return InventoryComponent;
}

void ADZ_RaidPlayerController::InitializeInventoryFromLoadout()
{
	if (!HasAuthority()) return;
	if (!IsValid(InventoryComponent)) return;
	
	UDZ_GameInstance* GameInstance = GetGameInstance<UDZ_GameInstance>();
	if (!IsValid(GameInstance)) return;
	
	const FDZ_Loadout& PendingLoadout = GameInstance->GetPendingLoadout();
	
	for (int32 SlotIndex = 0; SlotIndex < PendingLoadout.Equipments.Num(); SlotIndex++)
	{
		const int32 ItemId = PendingLoadout.Equipments[SlotIndex];
		if (ItemId == DZItem::EmptyId) continue;
		InventoryComponent->SetEquipments(SlotIndex, ItemId);
	}
	
	for (int32 SlotIndex = 0; SlotIndex < PendingLoadout.BagItems.Num(); SlotIndex++)
	{
		const FDZ_ItemStack& Item = PendingLoadout.BagItems[SlotIndex];
		if (Item.IsEmpty()) continue;
		if (!InventoryComponent->AddItem(SlotIndex, Item.ItemId, Item.Count))
		{
			UE_LOG(LogDZ, Warning, TEXT("Failed to restore bag item %d x%d at slot %d"), Item.ItemId, Item.Count, SlotIndex);
		}
	}
	
	InventoryComponent->OnItemsChanged.Broadcast();
	GameInstance->ClearPendingLoadout();
}

void ADZ_RaidPlayerController::TransferInventoryToContainer(UDZ_ContainerComponent* TargetContainer)
{
	if (!HasAuthority()) return;
	if (!IsValid(InventoryComponent)) return;
	if (!IsValid(TargetContainer)) return;
	
	TargetContainer->SetContents(InventoryComponent->GetAllItems());
	InventoryComponent->ClearAll();
}

bool ADZ_RaidPlayerController::BuildCharacterStats(FDZ_CharacterStatsInfo& OutStats) const
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

TWeakObjectPtr<UDZ_ContainerComponent> ADZ_RaidPlayerController::GetInteractContainer() const
{
	return InteractContainerComp;
}

void ADZ_RaidPlayerController::Skill1()
{
	ActivateSkill(0);
}

void ADZ_RaidPlayerController::Skill2()
{
	ActivateSkill(1);
}

void ADZ_RaidPlayerController::ActivateSkill(int32 SlotIndex)
{
	if (IsValid(RaidUI) && RaidUI->InteractController->IsBlockingAction()) return;
	if (!AbilitySystemComponent.IsValid()) return;
	
	const int32 SkillId = GetSlotSkill(SlotIndex);
	if (SkillId == DZSkill::EmptyId) return;
	
	const UDZ_GameInstance* GameInstance = Cast<UDZ_GameInstance>(GetGameInstance());
	if (!GameInstance) return;
	
	const FDZ_SkillData* SkillData = GameInstance->GetSkillData(SkillId);
	if (!SkillData) return;
	
	ActivateAbilityByTag(SkillData->AbilityTag);
}

void ADZ_RaidPlayerController::NotifyInteractFinished(AActor* Target, bool bSucceeded)
{
	ClientInteractFinished(Target, bSucceeded);
}

void ADZ_RaidPlayerController::HandleInventoryItemsChanged()
{
	RefreshUIAll(false);
}

void ADZ_RaidPlayerController::HandleContainerItemsChanged()
{
	RefreshUIAll(false);
}

void ADZ_RaidPlayerController::ActivateAbilityByTag(const FGameplayTag& AbilityTag)
{
	if (!AbilitySystemComponent.IsValid()) return;
	
	AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTag.GetSingleTagContainer());
}

void ADZ_RaidPlayerController::HandleInteractableActorChanged(AActor* Actor)
{
	if (!IsLocalController()) return;
	if (!IsValid(RaidUI)) return;
	
	RaidUI->InteractController->OnFocusInteractableChanged(Actor);
}

void ADZ_RaidPlayerController::AcknowledgePossession(APawn* InPawn)
{
	Super::AcknowledgePossession(InPawn);
	if (ADZ_PlayerCharacter* PlayerCharacter = Cast<ADZ_PlayerCharacter>(InPawn))
	{
		PlayerCharacter->OnFocusInteractableActorChanged.AddUniqueDynamic(this, &ADZ_RaidPlayerController::HandleInteractableActorChanged);
		PlayerCharacter->OnAttributesInitialized.AddUniqueDynamic(this, &ADZ_RaidPlayerController::RefreshStatusBars);
	}
}

void ADZ_RaidPlayerController::ServerCancelInteract_Implementation()
{
	UDZ_AbilitySystemComponent* ASC = Cast<UDZ_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	if (!ASC) return;


	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecByTag(DZGameplayTags::Ability_Interact);
	if (!Spec) return;

	ASC->CancelAbilityHandle(Spec->Handle);
}

void ADZ_RaidPlayerController::ServerTakeItem_Implementation(AActor* ContainerActor, int32 SlotIndex, int32 Count)
{
	if (!IsValid(ContainerActor)) return;

	UDZ_ContainerComponent* Container = ContainerActor->FindComponentByClass<UDZ_ContainerComponent>();
	if (!Container) return;
	
	UDZ_InventoryComponent* InventoryComp = GetInventoryComponent();
	if (!InventoryComp) return;
	
	if (ItemHelper::TakeItem(InventoryComp, Container, SlotIndex, Count))
	{
		RefreshUIAll(false);
	}
}

bool ADZ_RaidPlayerController::IsMovementLocked() const
{
	if (IsValid(RaidUI) && RaidUI->InteractController->IsBlockingAction()) return true;
	if (!AbilitySystemComponent.IsValid()) return false;
	if (AbilitySystemComponent->HasMatchingGameplayTag(DZGameplayTags::State_MovementLocked) || 
		AbilitySystemComponent->HasMatchingGameplayTag(DZGameplayTags::State_Interacting))
	{
		return true;
	}
	return false;
}

void ADZ_RaidPlayerController::Move(const FInputActionValue& Value)
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

void ADZ_RaidPlayerController::Look(const FInputActionValue& Value)
{
	const FVector2D InputVector = Value.Get<FVector2D>();
	AddYawInput(InputVector.X);
	AddPitchInput(InputVector.Y);
}

void ADZ_RaidPlayerController::Jump(const FInputActionValue& Value)
{
	if (IsMovementLocked()) return;
	if (!GetCharacter()) return;
	
	GetCharacter()->Jump();
	ShowTips(FText::FromString("Jumping"), 3);
}

void ADZ_RaidPlayerController::CloseTopWidget()
{
	if (!IsValid(RaidUI)) return;
	
	RaidUI->CloseTopWidget();
}

void ADZ_RaidPlayerController::OpenOrCloseInventoryPanel()
{
	if (!IsValid(RaidUI)) return;
	
	if (RaidUI->InventoryController->Toggle())
	{
		RefreshUIAll(false);
	}
}

FDZ_SettlementInfo ADZ_RaidPlayerController::BuildSettlementInfo() const
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
	
	if (UDZ_InventoryComponent* InventoryComp = GetInventoryComponent())
	{
		Info.Items = InventoryComp->GetAllItems();
		Info.Equipments = InventoryComp->GetEquipments();
		Info.TotalValue = InventoryComp->GetTotalValue();
	}
	return Info;
}

void ADZ_RaidPlayerController::ReturnToLobby()
{
	ServerReturnToLobby();
}

void ADZ_RaidPlayerController::ServerReturnToLobby_Implementation()
{
	UDZ_GameInstance* GameInstance = GetGameInstance<UDZ_GameInstance>();
	if (!IsValid(GameInstance)) return;
	
	const FName LobbyMapName = GameInstance->GetLobbyMapName();
	if (LobbyMapName.IsNone())
	{
		UE_LOG(LogDZ, Warning, TEXT("LobbyMapName is not configured on GameInstance"));
		return;
	}
	
	GetWorld()->ServerTravel(LobbyMapName.ToString());
}
