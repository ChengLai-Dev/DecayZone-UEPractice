// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/DZ_BlueprintLibrary.h"

#include "Engine/OverlapResult.h"

void UDZ_BlueprintLibrary::SetActorHighlight(AActor* Actor, bool bEnable)
{
	if (!IsValid(Actor)) return;
	
	TArray<UMeshComponent*> MeshComponents;
	Actor->GetComponents<UMeshComponent>(MeshComponents);
	
	for (UMeshComponent* Mesh : MeshComponents)
	{
		Mesh->SetRenderCustomDepth(bEnable);
		if (bEnable)
		{
			Mesh->SetCustomDepthStencilValue(1);
		}
	}
}

void UDZ_BlueprintLibrary::MeleeTraceHit(AActor* Actor, TArray<AActor*>& OutHitActors, float Range, float AttackAngle, bool bDebug)
{
	OutHitActors.Empty();
	if (!IsValid(Actor)) return;

	UWorld* World = Actor->GetWorld();
	if (!IsValid(World) || Range <= 0.f) return;

	const FVector Origin = Actor->GetActorLocation();
	const FVector Forward = Actor->GetActorForwardVector().GetSafeNormal2D();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Actor);
	Params.bTraceComplex = false;

	TArray<FOverlapResult> Overlaps;
	const FCollisionShape SearchShape = FCollisionShape::MakeSphere(Range);
	
	World->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ECC_Pawn, SearchShape, Params);
	// 扇形角度一半的余弦；点积小于它就在扇形外
	const float HalfAngleCos = FMath::Cos(FMath::DegreesToRadians(AttackAngle * 0.5f));

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* OtherActor = Overlap.GetActor();
		if (!IsValid(OtherActor) || OtherActor == Actor) continue;
		if (OutHitActors.Contains(OtherActor)) continue;
		if (IsFriendly(Actor, OtherActor)) continue;

		if (AttackAngle < 360.f)
		{
			const FVector ToTarget = OtherActor->GetActorLocation() - Origin;
			if (FVector::DotProduct(Forward, ToTarget.GetSafeNormal2D()) < HalfAngleCos) continue;
		}

		OutHitActors.Add(OtherActor);
	}
	// if (bDebug)
	// {
	// 	DrawDebugCircleArc(Actor->GetWorld(), Actor->GetActorLocation(), Range, 
	// 		Forward, AttackAngle * 0.5f, 16, FColor::Blue, false, 3, 0, 2.f);
	// 	const FVector LeftVector = Forward.RotateAngleAxis(-AttackAngle * 0.5f, FVector::UpVector);
	// 	const FVector RightVector = Forward.RotateAngleAxis(AttackAngle * 0.5f, FVector::UpVector);
	// 	DrawDebugLine(Actor->GetWorld(), Actor->GetActorLocation(), Actor->GetActorLocation() + LeftVector * Range, 
	// 		FColor::Blue, false, 3, 0, 2);
	// 	DrawDebugLine(Actor->GetWorld(), Actor->GetActorLocation(), Actor->GetActorLocation() + RightVector * Range, 
	// 		FColor::Blue, false, 3, 0, 2);
	// }
}

bool UDZ_BlueprintLibrary::IsFriendly(AActor* Actor1, AActor* Actor2)
{
	return false;
}
