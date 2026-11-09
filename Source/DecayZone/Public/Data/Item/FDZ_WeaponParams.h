#pragma once

#include "CoreMinimal.h"
#include "FDZ_WeaponParams.generated.h"

USTRUCT(BlueprintType)
struct FDZ_WeaponParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	float Damage = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	float Range = 20.f;
	
	UPROPERTY(BlueprintReadOnly)
	float AttackAngle = 120.f;
	
	UPROPERTY(BlueprintReadOnly)
	float AttackCooldown = 0.f;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(BlueprintReadOnly)
	bool bAutomatic = false;
};
