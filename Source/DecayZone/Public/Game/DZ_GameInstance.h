// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DZ_GameInstance.generated.h"

class UDZ_ContainerSpawnItemConfig;
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
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ItemDataTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> SkillDataTable;
	
	TMap<int32, const FDZ_ItemData*> ItemMap;
	TMap<int32, const FDZ_SkillData*> SkillMap;
};
