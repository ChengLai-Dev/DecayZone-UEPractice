// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_AbilityBarController.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Data/Ability/DZ_SkillDefines.h"
#include "Data/Ability/FDZ_SkillData.h"
#include "Data/UI/DZ_UIConfig.h"
#include "Data/UI/FDZ_AbilitySlotLayout.h"
#include "DecayZone/DecayZone.h"
#include "Game/DZ_GameInstance.h"
#include "Player/DZ_PlayerController.h"
#include "Player/DZ_PlayerState.h"
#include "UI/Widgets/HUD/DZ_AbilityBar.h"

namespace
{
	constexpr float AbilityBarRefreshInterval = 0.1f;
}

void UDZ_AbilityBarController::Initialize(ADZ_PlayerController* InPlayerController)
{
	Super::Initialize(InPlayerController);
	if (!IsValid(UIConfig)) return;
	
	AbilityBarWidget = CreateWidget<UDZ_AbilityBar>(PlayerController, UIConfig->AbilityBarClass);
	if (!IsValid(AbilityBarWidget)) return;
	
	SlotSkillIds.Reset();
	for (const FDZ_AbilitySlotLayout& SlotLayout : UIConfig->AbilitySlots)
	{
		SlotSkillIds.Add(SlotLayout.DefaultSkillId);
	}
	
	if (SlotSkillIds.Num() > UDZ_AbilityBar::MaxSlots)
	{
		UE_LOG(LogDZ, Warning, TEXT("AbilitySlots has %d entries, only %d slots are supported"), SlotSkillIds.Num(), UDZ_AbilityBar::MaxSlots);
	}
	
	RefreshAbilityBar();
	AbilityBarWidget->Open();
}

void UDZ_AbilityBarController::Shutdown()
{
	if (IsValid(AbilityBarWidget))
	{
		AbilityBarWidget->RemoveFromParent();
	}
	AbilityBarWidget = nullptr;
	SlotSkillIds.Reset();
	RefreshAccum = 0.f;
	Super::Shutdown();
}

void UDZ_AbilityBarController::Tick(float DeltaSeconds)
{
	RefreshAccum += DeltaSeconds;
	if (RefreshAccum < AbilityBarRefreshInterval) return;
	
	RefreshAccum = 0.f;
	RefreshAbilityBar();
}

void UDZ_AbilityBarController::SetSlotSkill(int32 SlotIndex, int32 SkillId)
{
	if (!SlotSkillIds.IsValidIndex(SlotIndex)) return;
	if (SlotSkillIds[SlotIndex] == SkillId) return;
	
	if (SkillId != DZSkill::EmptyId)
	{
		const int32 ExistingIndex = SlotSkillIds.IndexOfByKey(SkillId);
		if (ExistingIndex != INDEX_NONE && ExistingIndex != SlotIndex)
		{
			SlotSkillIds[ExistingIndex] = SlotSkillIds[SlotIndex];
			RefreshAbilitySlot(ExistingIndex);
		}
	}
	
	SlotSkillIds[SlotIndex] = SkillId;
	RefreshAbilitySlot(SlotIndex);
}

int32 UDZ_AbilityBarController::GetSlotSkill(int32 SlotIndex) const
{
	return SlotSkillIds.IsValidIndex(SlotIndex) ? SlotSkillIds[SlotIndex] : DZSkill::EmptyId;
}

void UDZ_AbilityBarController::Hide()
{
	if (IsValid(AbilityBarWidget))
	{
		AbilityBarWidget->Hide();
	}
}

void UDZ_AbilityBarController::RefreshAbilityBar()
{
	if (!IsValid(AbilityBarWidget)) return;
	
	const int32 SlotCount = FMath::Min(SlotSkillIds.Num(), UDZ_AbilityBar::MaxSlots);
	for (int32 i = 0; i < SlotCount; ++i)
	{
		RefreshAbilitySlot(i);
	}
}

void UDZ_AbilityBarController::RefreshAbilitySlot(int32 SlotIndex)
{
	if (!IsValid(AbilityBarWidget)) return;
	if (!SlotSkillIds.IsValidIndex(SlotIndex)) return;
	
	FText KeyText;
	if (IsValid(UIConfig) && UIConfig->AbilitySlots.IsValidIndex(SlotIndex))
	{
		KeyText = UIConfig->AbilitySlots[SlotIndex].KeyText;
	}
	
	const FDZ_SkillData* SkillData = IsValid(GameInstance) ? GameInstance->GetSkillData(SlotSkillIds[SlotIndex]) : nullptr;
	if (!SkillData)
	{
		AbilityBarWidget->SetKeyText(SlotIndex, KeyText);
		AbilityBarWidget->SetEmpty(SlotIndex, true);
		return;
	}
	
	AbilityBarWidget->SetEmpty(SlotIndex, false);
	AbilityBarWidget->SetKeyText(SlotIndex, KeyText);
	AbilityBarWidget->SetIcon(SlotIndex, SkillData->Icon.Get());
	
	float CooldownPercent = 0.f;
	float RemainingSeconds = 0.f;
	bool bAvailable = false;
	bool bActive = false;
	
	if (UDZ_AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		if (AbilitySystemComponent->AbilityActorInfo.IsValid())
		{
			FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecByTag(SkillData->AbilityTag);
			if (Spec && Spec->Ability)
			{
				const FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get();
				float CooldownDuration = 0.f;
				Spec->Ability->GetCooldownTimeRemainingAndDuration(Spec->Handle, ActorInfo, RemainingSeconds, CooldownDuration);
				CooldownPercent = CooldownDuration > 0.f ? RemainingSeconds / CooldownDuration : 0.f;
				bAvailable = Spec->Ability->CheckCost(Spec->Handle, ActorInfo);
				bActive = Spec->IsActive();
			}
		}
	}
	
	AbilityBarWidget->SetCooldown(SlotIndex, CooldownPercent, RemainingSeconds);
	AbilityBarWidget->SetAvailable(SlotIndex, bAvailable);
	AbilityBarWidget->SetActive(SlotIndex, bActive);
}

UDZ_AbilitySystemComponent* UDZ_AbilityBarController::GetAbilitySystemComponent() const
{
	if (!PlayerController) return nullptr;
	
	ADZ_PlayerState* DZPlayerState = PlayerController->GetPlayerState<ADZ_PlayerState>();
	return DZPlayerState ? Cast<UDZ_AbilitySystemComponent>(DZPlayerState->GetAbilitySystemComponent()) : nullptr;
}
