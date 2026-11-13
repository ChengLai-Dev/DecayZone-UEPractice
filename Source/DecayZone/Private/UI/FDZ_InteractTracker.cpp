// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FDZ_InteractTracker.h"

void FDZ_InteractTracker::Begin(AActor* InTarget, float InDuration, bool bInAuto)
{
	Target = InTarget;
	Duration = FMath::Max(0.f, InDuration);
	Elapsed = 0.f;
	bAuto = bInAuto;
	bActive = true;
}

void FDZ_InteractTracker::Tick(float DeltaSeconds)
{
	if (!bActive) return;

	Elapsed = FMath::Min(Elapsed + DeltaSeconds, Duration);
}

void FDZ_InteractTracker::Reset()
{
	Target = nullptr;
	Duration = 0.f;
	Elapsed = 0.f;
	bActive = false;
	bAuto = false;
}

float FDZ_InteractTracker::GetProgress() const
{
	if (Duration <= 0.f) return 1.f;

	return FMath::Clamp(Elapsed / Duration, 0.f, 1.f);
}

float FDZ_InteractTracker::GetRemaining() const
{
	return FMath::Max(0.f, Duration - Elapsed);
}
