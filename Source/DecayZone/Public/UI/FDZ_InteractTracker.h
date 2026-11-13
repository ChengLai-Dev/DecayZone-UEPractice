// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FDZ_InteractTracker
{
public:
	void Begin(AActor* InTarget, float InDuration, bool bInAuto);
	void Tick(float DeltaSeconds);
	void Reset();
	
	float GetProgress() const;
	float GetRemaining() const;

	bool IsActive() const { return bActive; }
	bool IsAuto() const { return bAuto; }
	AActor* GetTarget() const { return Target.Get(); }

private:
	TWeakObjectPtr<AActor> Target;

	float Duration = 0.f;
	float Elapsed = 0.f;
	bool bActive = false;
	bool bAuto = false;
};
