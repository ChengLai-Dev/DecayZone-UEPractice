// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Controllers/DZ_StatusBarsController.h"

#include "AbilitySystem/DZ_AbilitySystemComponent.h"
#include "AbilitySystem/DZ_AttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "Player/DZ_PlayerState.h"
#include "UI/Widgets/HUD/DZ_StatusBars.h"

void UDZ_StatusBarsController::SetStatusBarsWidget(UDZ_StatusBars* InWidget)
{
	StatusBarsWidget = InWidget;
	Refresh();
}

void UDZ_StatusBarsController::Shutdown()
{
	StatusBarsWidget = nullptr;
	Super::Shutdown();
}

void UDZ_StatusBarsController::Refresh()
{
	if (!IsValid(StatusBarsWidget)) return;

	UDZ_AbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent();
	if (!AbilitySystemComponent) return;

	const float Health = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetHealthAttribute());
	const float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxHealthAttribute());
	const float Energy = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetEnergyAttribute());
	const float MaxEnergy = AbilitySystemComponent->GetNumericAttribute(UDZ_AttributeSet::GetMaxEnergyAttribute());

	StatusBarsWidget->SetHealthInfo(Health, MaxHealth);
	StatusBarsWidget->SetEnergyInfo(Energy, MaxEnergy);
}

UDZ_AbilitySystemComponent* UDZ_StatusBarsController::GetAbilitySystemComponent() const
{
	if (!PlayerController) return nullptr;

	ADZ_PlayerState* DZPlayerState = PlayerController->GetPlayerState<ADZ_PlayerState>();
	return DZPlayerState ? Cast<UDZ_AbilitySystemComponent>(DZPlayerState->GetAbilitySystemComponent()) : nullptr;
}
