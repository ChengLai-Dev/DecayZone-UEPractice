// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "UI/Controllers/DZ_UIControllerBase.h"
#include "DZ_StashController.generated.h"

class ADZ_LobbyPlayerController;
class UDZ_StashPanel;

UCLASS()
class DECAYZONE_API UDZ_StashController : public UDZ_UIControllerBase
{
	GENERATED_BODY()

public:
	virtual void Initialize(APlayerController* InPlayerController) override;
	virtual void Shutdown() override;

	void Open();
	void Close();
	bool Toggle();
	void Refresh();

	UPROPERTY(BlueprintAssignable)
	FOnEvent OnDepartRequested;

private:
	UFUNCTION()
	void HandleItemMoveRequested(UWidget* From, UWidget* To);

	UFUNCTION()
	void HandleStartRequested();

	UFUNCTION()
	void HandleShopRequested();

	UFUNCTION()
	void HandleSellTargetChanged(FDZ_ItemSlotHandle Handle);

	UFUNCTION()
	void HandleSellRequested(FDZ_ItemSlotHandle Handle);

	UFUNCTION()
	void HandleStashChanged();

	UPROPERTY()
	TObjectPtr<UDZ_StashPanel> StashPanelWidget;
	
	UPROPERTY()
	TObjectPtr<ADZ_LobbyPlayerController> LobbyPlayerController;
};
