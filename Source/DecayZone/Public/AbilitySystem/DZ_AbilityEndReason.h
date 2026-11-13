// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DZ_AbilityEndReason.generated.h"

/**
 * 能力结束原因。
 * EndAbility 的 bWasCancelled 按引擎惯例只区分「正常结束 / 非正常结束」，
 * 具体是哪一种非正常，由这个枚举说明。
 */
UENUM(BlueprintType)
enum class EDZAbilityEndReason : uint8
{
	/** 未决定：解析时交给 bWasCancelled 推断 */
	None		UMETA(DisplayName = "None"),

	/** 正常跑完（默认值） */
	Completed	UMETA(DisplayName = "Completed"),

	/** 被打断：外部 CancelAbility，或能力自取消 */
	Cancelled	UMETA(DisplayName = "Cancelled"),

	/** 跑完了但没成：判定没过、前置条件不满足等 */
	Failed		UMETA(DisplayName = "Failed"),
};
