#pragma once

#include "CoreMinimal.h"
#include "Data/Ability/DZ_SkillDefines.h"
#include "FDZ_AbilitySlotLayout.generated.h"

USTRUCT(BlueprintType)
struct FDZ_AbilitySlotLayout
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText KeyText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 DefaultSkillId = DZSkill::EmptyId;
};
