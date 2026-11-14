#pragma once

#include "CoreMinimal.h"
#include "Data/Ability/DZ_SkillDefines.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "FDZ_SkillData.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FDZ_SkillData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SkillId = DZSkill::EmptyId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;
};
