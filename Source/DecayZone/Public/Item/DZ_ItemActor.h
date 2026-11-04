// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DZ_ItemActor.generated.h"

struct FDZ_ItemData;

UCLASS()
class DECAYZONE_API ADZ_ItemActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADZ_ItemActor();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DecayZone|Item")
	int32 ItemID;
	
	const FDZ_ItemData* GetData() const;

};
