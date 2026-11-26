#pragma once

#include "CoreMinimal.h"
#include "Data/Item/FDZ_AttackData.h"
#include "Data/ItemType/DZ_EquipmentData.h"
#include "Templates/SubclassOf.h"
#include "DZ_WeaponData.generated.h"

class AActor;
class UGameplayAbility;

UCLASS(Abstract, BlueprintType)
class DECAYZONE_API UWeaponData : public UEquipmentData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	FDZ_AttackData AttackData;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AActor> WeaponActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UGameplayAbility> AttackAbilityClass;
};
