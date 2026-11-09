// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DZ_BlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DECAYZONE_API UDZ_BlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public: 
	UFUNCTION(BlueprintCallable)
	static void SetActorHighlight(AActor* Actor, bool bEnable);
	
	UFUNCTION(BlueprintCallable)
	static void MeleeTraceHit(AActor* Actor, TArray<AActor*>& OutHitActors, float Range = 200, float AttackAngle = 120, bool bDebug = false);
	
	UFUNCTION(BlueprintCallable)
	static bool IsFriendly(AActor* Actor1, AActor* Actor2);
};
