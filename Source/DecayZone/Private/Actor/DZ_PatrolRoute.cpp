// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/DZ_PatrolRoute.h"

#include "Components/SplineComponent.h"


// Sets default values
ADZ_PatrolRoute::ADZ_PatrolRoute()
{
	PrimaryActorTick.bCanEverTick = false;
	PatrolRoute = CreateDefaultSubobject<USplineComponent>("PatrolRoute");
}

FVector ADZ_PatrolRoute::GetNextPatrolPoint()
{
	const int32 PointsNum = PatrolRoute->GetNumberOfSplinePoints();
	const FVector& PointLocation = PatrolRoute->GetLocationAtSplinePoint(CurIndex, ESplineCoordinateSpace::World);
	if (CurIndex + Direction >= PointsNum || CurIndex + Direction < 0)
	{
		Direction = -Direction;
	}
	CurIndex += Direction;
	return  PointLocation;
}



