// Rogue Rigs - shotgun: each shot fires PelletCount piercing pellets spread evenly over +/-SpreadDeg around the aim, each with knockback.

#include "Weapons/RRShotgun.h"

#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Player/RRMiner.h"
#include "Projectiles/RRProjectile.h"
#include "Projectiles/RRShotgunPellet.h"
#include "RogueRigs.h"
#include "Weapons/RRWeaponStatsRow.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRShotgun)

ARRShotgun::ARRShotgun(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Shotgun");
	ProjectileClass = ARRShotgunPellet::StaticClass();
}

void ARRShotgun::ApplyStatsRow(const FRRWeaponStatsRow& Row)
{
	Super::ApplyStatsRow(Row);
	PelletCount = FMath::Max(Row.PelletCount, 1);
	KnockbackForce = FMath::Max(Row.KnockbackForce, 0.f);
}

float ARRShotgun::GetPelletYawOffset(int32 PelletIndex) const
{
	if (PelletCount <= 1)
	{
		return 0.f;
	}
	const float Alpha = static_cast<float>(FMath::Clamp(PelletIndex, 0, PelletCount - 1)) / static_cast<float>(PelletCount - 1);
	return FMath::Lerp(-SpreadDeg, SpreadDeg, Alpha);
}

void ARRShotgun::FireOnce()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	if (!ProjectileClass)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: ProjectileClass is not set; shot skipped."), *GetName());
		return;
	}
	if (ProjectileSpeed <= 0.f || Range <= 0.f)
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: ProjectileSpeed %.1f or Range %.1f is not positive; shot skipped."), *GetName(), ProjectileSpeed, Range);
		return;
	}

	// One spec per shot (source attributes snapshot now); each pellet hit is executed on its own copy, so each rolls its own critical.
	const FGameplayEffectSpecHandle Spec = MakeDamageSpec();
	if (!Spec.IsValid())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: no damage spec (DamageEffectClass unset); the pellets will deal no damage."), *GetName());
	}

	// task-412 Q4: the shotgun has no hand socket, so its pellets leave from the shown weapon's Muzzle (burst rifle) with the same
	// capsule and miner-to-Muzzle sweep as the rifle bullets (ARRWeapon::GetShotPlacement); one placement for the 7 pellets.
	const FRRShotPlacement Placement = GetShotPlacement();
	const FVector Direction = GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	const float AimYaw = Direction.Rotation().Yaw;
	const int32 MaxHits = GetPierceCount();

	LastPelletsSpawned = 0;
	for (int32 Index = 0; Index < PelletCount; ++Index)
	{
		const FRotator PelletRotation(0.f, AimYaw + GetPelletYawOffset(Index), 0.f);
		if (SpawnShot(Placement, PelletRotation, Spec, MaxHits, KnockbackForce))
		{
			++LastPelletsSpawned;
		}
	}
	++ShotCount;

	OnShotFired(Placement.Origin, Direction, SpreadDeg);
}
