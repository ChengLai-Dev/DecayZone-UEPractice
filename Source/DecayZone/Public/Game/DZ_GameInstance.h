// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_DelegateTypes.h"
#include "Data/Item/FDZ_ItemStack.h"
#include "Data/Item/FDZ_Loadout.h"
#include "Engine/GameInstance.h"
#include "DZ_GameInstance.generated.h"

class UDZ_ContainerSpawnItemConfig;
enum class EItemContainer : uint8;
struct FDZ_ItemSlotHandle;
struct FDZ_ItemData;
struct FDZ_SkillData;
class UDZ_StartupAbilities;
class UDZ_UIConfig;

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_GameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	virtual void Init() override;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDZ_StartupAbilities> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDZ_ContainerSpawnItemConfig> ContainerSpawnItemConfig;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|UI")
	TObjectPtr<UDZ_UIConfig> UIConfig;
	
	const FDZ_ItemData* GetItemData(int32 ItemId);
	const FDZ_SkillData* GetSkillData(int32 SkillId) const;
	
	/** 槽位 SkillId，索引即槽位号 */
	int32 GetSlotSkillId(int32 SlotIndex) const;
	void SetSlotSkillId(int32 SlotIndex, int32 SkillId);
	int32 GetSlotSkillCount() const;
	
	/** 按 UIConfig 的默认配置填充槽位，只在 Init 时调用一次 */
	void InitSlotSkills();
	
	const TArray<FDZ_ItemStack>& GetStashItems() const { return StashItems; }
	const TArray<FDZ_ItemStack>& GetLoadoutBagItems() const { return LoadoutBagItems; }
	const TArray<int32>& GetLoadoutEquipments() const { return LoadoutEquipments; }
	
	int32 GetStashCapacity() const { return StashCapacity; }
	int32 GetStashBagCapacity() const { return StashBagCapacity; }
	int32 GetStashUsedSlots() const;
	int32 GetStashBagUsedSlots() const;
	int32 GetStashTotalValue();
	
	bool MoveStashItem(const FDZ_ItemSlotHandle& From, const FDZ_ItemSlotHandle& To, FText& OutFailReason);
	
	UPROPERTY(BlueprintAssignable, Category = "DecayZone|Stash")
	FOnEvent OnStashChanged;
	
	bool TakeDepartureLoadout();
	
	void DepositRaidResult(const TArray<FDZ_ItemStack>& Items, const TArray<int32>& Equipments);
	
	const FDZ_Loadout& GetPendingLoadout() const { return PendingLoadout; }
	void SetPendingLoadout(const FDZ_Loadout& InLoadout);
	void ClearPendingLoadout();
	
	FName GetRaidMapName() const { return RaidMapName; }
	FName GetLobbyMapName() const { return LobbyMapName; }
	
private:
	UPROPERTY(BlueprintReadOnly, Category = "DecayZone|Skills", meta = (AllowPrivateAccess = "true"))
	TArray<int32> SlotSkillIds;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Stash")
	int32 StashCapacity = 30;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Stash")
	int32 StashBagCapacity = 6;
	
	UPROPERTY()
	TArray<FDZ_ItemStack> StashItems;
	
	UPROPERTY()
	TArray<int32> LoadoutEquipments;
	
	UPROPERTY()
	TArray<FDZ_ItemStack> LoadoutBagItems;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Levels")
	FName RaidMapName;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Levels")
	FName LobbyMapName;
	
	UPROPERTY(EditDefaultsOnly, Category = "DecayZone|Stash")
	TArray<FDZ_ItemStack> DebugStashItems;
	
	UPROPERTY()
	FDZ_Loadout PendingLoadout;
	
	const TArray<FDZ_ItemStack>* GetStashStackArray(EItemContainer Container) const;
	TArray<FDZ_ItemStack>* GetStashStackArray(EItemContainer Container);
	bool IsStashSlotValid(const FDZ_ItemSlotHandle& Handle) const;
	FDZ_ItemStack GetStashSlotItem(const FDZ_ItemSlotHandle& Handle) const;
	void SetStashSlotItem(const FDZ_ItemSlotHandle& Handle, const FDZ_ItemStack& Item);
	void ClearStashSlot(const FDZ_ItemSlotHandle& Handle);
	bool CanAcceptStashItem(const FDZ_ItemSlotHandle& Handle, int32 ItemId, int32 Count);
	
	bool TryAddItemToStash(const FDZ_ItemStack& Item);
	bool TryAddEquipmentToStash(int32 ItemId);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ItemDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> SkillDataTable;
	
	TMap<int32, const FDZ_ItemData*> ItemMap;
	TMap<int32, const FDZ_SkillData*> SkillMap;
};
