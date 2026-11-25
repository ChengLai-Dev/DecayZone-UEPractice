#include "AbilitySystem/Abilities/Attack/Ranged/DZ_RangedWeaponAttack.h"

#include "Components/SceneComponent.h"
#include "Data/ItemType/Weapon/DZ_RangedWeaponData.h"
#include "GameFramework/Pawn.h"

bool UDZ_RangedWeaponAttack::ResolveAttackData(FDZ_AttackData& OutAttackData) const
{
	const URangedWeaponData* RangedWeaponData = Cast<URangedWeaponData>(ResolveWeaponData());
	if (!RangedWeaponData) return false;

	OutAttackData = RangedWeaponData->AttackData;
	return true;
}

void UDZ_RangedWeaponAttack::PerformAttack(const FDZ_AttackData& AttackData)
{
	const URangedWeaponData* RangedWeaponData = Cast<URangedWeaponData>(ResolveWeaponData());
	if (!RangedWeaponData) return;

	FVector Start;
	FVector Direction;
	if (!GetAimRay(Start, Direction, *RangedWeaponData)) return;

	if (RangedWeaponData->SpreadAngle > 0.f)
	{
		Direction = FMath::VRandCone(Direction, FMath::DegreesToRadians(RangedWeaponData->SpreadAngle * 0.5f));
	}

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetAvatarActorFromActorInfo());

	FHitResult HitResult;
	const FVector End = Start + Direction * AttackData.Range;
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Pawn, Params)) return;

	ApplyAttackDamage(HitResult.GetActor(), AttackData);
}

bool UDZ_RangedWeaponAttack::GetAimRay(FVector& OutStart, FVector& OutDirection, const URangedWeaponData& WeaponData) const
{
	const APawn* Avatar = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!Avatar) return false;

	const AController* Controller = Avatar->GetController();
	if (!Controller) return false;

	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

	OutStart = ViewLocation;
	OutDirection = ViewRotation.Vector();

	if (WeaponData.MuzzleSocketName.IsNone()) return true;

	TArray<AActor*> AttachedActors;
	Avatar->GetAttachedActors(AttachedActors);

	for (const AActor* Actor : AttachedActors)
	{
		if (!IsValid(Actor)) continue;

		TArray<USceneComponent*> Components;
		Actor->GetComponents<USceneComponent>(Components);

		for (const USceneComponent* Component : Components)
		{
			if (!Component || !Component->DoesSocketExist(WeaponData.MuzzleSocketName)) continue;

			OutStart = Component->GetSocketLocation(WeaponData.MuzzleSocketName);
			return true;
		}
	}

	return true;
}
