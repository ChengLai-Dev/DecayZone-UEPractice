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
	
	static void SetActorHighlight(AActor* Actor, bool bEnable);

};
