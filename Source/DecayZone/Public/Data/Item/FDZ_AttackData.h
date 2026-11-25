#pragma once

#include "CoreMinimal.h"
#include "FDZ_AttackData.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FDZ_AttackData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Damage = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Range = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackCooldown = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> AttackMontage;
};
