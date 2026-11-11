// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DZ_PatrolRoute.generated.h"

class USplineComponent;

UCLASS()
class DECAYZONE_API ADZ_PatrolRoute : public AActor
{
	GENERATED_BODY()

public:

	ADZ_PatrolRoute();

	FVector GetNextPatrolPoint();
	
private:
	int32 CurIndex = 0;
	int32 Direction = 1;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USplineComponent> PatrolRoute;
};
