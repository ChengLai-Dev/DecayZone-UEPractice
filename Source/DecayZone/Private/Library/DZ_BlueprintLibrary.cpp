// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/DZ_BlueprintLibrary.h"

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