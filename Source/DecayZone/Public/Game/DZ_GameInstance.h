// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DZ_GameInstance.generated.h"

struct FDZ_ItemData;
class UDZ_StartupAbilities;
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
	
	const FDZ_ItemData* GetItemData(int32 ItemId);
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> ItemDataTable;
	
	TMap<int32, const FDZ_ItemData*> ItemMap;
};
