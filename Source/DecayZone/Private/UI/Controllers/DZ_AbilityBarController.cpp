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
	
	if (UIConfig->AbilitySlots.Num() > UDZ_AbilityBar::MaxSlots)
	{
		UE_LOG(LogDZ, Warning, TEXT("AbilitySlots has %d entries, only %d slots are supported"), UIConfig->AbilitySlots.Num(), UDZ_AbilityBar::MaxSlots);
	}
	
	Refresh();
	AbilityBarWidget->Open();
}

void UDZ_AbilityBarController::Shutdown()
{
	if (IsValid(AbilityBarWidget))
	{
		AbilityBarWidget->RemoveFromParent();
	}
	AbilityBarWidget = nullptr;
	RefreshAccum = 0.f;
	Super::Shutdown();
}

void UDZ_AbilityBarController::Tick(float DeltaSeconds)
{
	RefreshAccum += DeltaSeconds;
	if (RefreshAccum < AbilityBarRefreshInterval) return;
	
	RefreshAccum = 0.f;
	RefreshSlotsDynamic();
}

void UDZ_AbilityBarController::SetSlotSkill(int32 SlotIndex, int32 SkillId)
{
	if (!IsValid(GameInstance)) return;
	if (SlotIndex < 0 || SlotIndex >= GameInstance->GetSlotSkillCount()) return;
	
	GameInstance->SetSlotSkillId(SlotIndex, SkillId);
	Refresh();
}

int32 UDZ_AbilityBarController::GetSlotSkill(int32 SlotIndex) const
{
	if (!IsValid(GameInstance)) return DZSkill::EmptyId;
	
	return GameInstance->GetSlotSkillId(SlotIndex);
}

void UDZ_AbilityBarController::Refresh()
{
	RefreshSlotsStatic();
	RefreshSlotsDynamic();
}

void UDZ_AbilityBarController::Hide()
{
	if (IsValid(AbilityBarWidget))
	{
		AbilityBarWidget->Hide();
	}
}

void UDZ_AbilityBarController::RefreshSlotsStatic()
{
	if (!IsValid(AbilityBarWidget)) return;
	if (!IsValid(GameInstance)) return;
	
	const int32 SlotCount = FMath::Min(GameInstance->GetSlotSkillCount(), UDZ_AbilityBar::MaxSlots);
	for (int32 i = 0; i < SlotCount; ++i)
	{
		RefreshSlotStatic(i);
	}
}

void UDZ_AbilityBarController::RefreshSlotsDynamic()
{
	if (!IsValid(AbilityBarWidget)) return;
	if (!IsValid(GameInstance)) return;
	
	const int32 SlotCount = FMath::Min(GameInstance->GetSlotSkillCount(), UDZ_AbilityBar::MaxSlots);
	for (int32 i = 0; i < SlotCount; ++i)
	{
		RefreshSlotDynamic(i);
	}
}

void UDZ_AbilityBarController::RefreshSlotStatic(int32 SlotIndex)
{
	if (!IsValid(AbilityBarWidget)) return;
	if (!IsValid(GameInstance)) return;
	if (SlotIndex < 0 || SlotIndex >= GameInstance->GetSlotSkillCount()) return;
	
	FText KeyText;
	if (IsValid(UIConfig) && UIConfig->AbilitySlots.IsValidIndex(SlotIndex))
	{
		KeyText = UIConfig->AbilitySlots[SlotIndex].KeyText;
	}
	AbilityBarWidget->SetKeyText(SlotIndex, KeyText);
	
	const FDZ_SkillData* SkillData = GameInstance->GetSkillData(GameInstance->GetSlotSkillId(SlotIndex));
	AbilityBarWidget->SetEmpty(SlotIndex, !SkillData);
	if (!SkillData) return;
	
	AbilityBarWidget->SetIcon(SlotIndex, SkillData->Icon.Get());
}

void UDZ_AbilityBarController::RefreshSlotDynamic(int32 SlotIndex)
{
	if (!IsValid(AbilityBarWidget)) return;
	if (!IsValid(GameInstance)) return;
	if (SlotIndex < 0 || SlotIndex >= GameInstance->GetSlotSkillCount()) return;
	
	const FDZ_SkillData* SkillData = GameInstance->GetSkillData(GameInstance->GetSlotSkillId(SlotIndex));
	if (!SkillData) return;
	
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
