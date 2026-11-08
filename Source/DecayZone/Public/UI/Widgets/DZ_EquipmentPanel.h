// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/FDZ_ItemSlotHandle.h"
#include "UI/DZ_UserWidget.h"
#include "DZ_EquipmentPanel.generated.h"

class UDZ_CharacterStats;
enum class EEquipSlot : uint8;
class UDZ_EquipmentSlot;
/**
 * 
 */

USTRUCT()
struct FDZ_CharacterStatsInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	int32 Health;
	
	UPROPERTY(BlueprintReadOnly)
	int32 MaxHealth;
	
	UPROPERTY(BlueprintReadOnly)
	int32 Weight;
	
	UPROPERTY(BlueprintReadOnly)
	int32 MaxWeight;
	
	UPROPERTY(BlueprintReadOnly)
	int32 Energy;
	
	UPROPERTY(BlueprintReadOnly)
	int32 MaxEnergy;
	
	UPROPERTY(BlueprintReadOnly)
	int32 Armor;
};

UCLASS()
class DECAYZONE_API UDZ_EquipmentPanel : public UDZ_UserWidget
{
	GENERATED_BODY()
public:
	
	virtual void NativeOnInitialized() override;
	
	void SetItemInfo(int32 ItemId, EEquipSlot EquipSlot);
	void SetEquipmentInfo(const TArray<int32>& Equipments);
	void SetCharacterStatsInfo(const FDZ_CharacterStatsInfo& StatsInfo);
	
	UPROPERTY(BlueprintAssignable)
	FOnWidgetDragEndedEvent OnItemDrop;
	
private:
	
	UFUNCTION()
	void HandleEquipSlotDrop(UWidget* From, UWidget* To);

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Armor;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Backpack;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Melee;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_EquipmentSlot> EquipSlot_Primary;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDZ_CharacterStats> CharacterStats;
	
	TMap<EEquipSlot, TWeakObjectPtr<UDZ_EquipmentSlot>> EquipSlotMap;
};
