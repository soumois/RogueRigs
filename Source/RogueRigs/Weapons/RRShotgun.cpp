// Rogue Rigs - shotgun: instant conical wave hitting every robot in the cone with a clear grid line of sight, with knockback.

#include "Weapons/RRShotgun.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemies/RREnemy.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Player/RRMiner.h"
#include "RogueRigs.h"
#include "Weapons/RRWeaponStatsRow.h"
#include "World/RRWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRShotgun)

ARRShotgun::ARRShotgun(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StatsRowName = TEXT("Shotgun");
}

void ARRShotgun::ApplyStatsRow(const FRRWeaponStatsRow& Row)
{
	Super::ApplyStatsRow(Row);
	ConeAngleDeg = Row.ConeAngleDeg;
	KnockbackForce = Row.KnockbackForce;
}

FVector ARRShotgun::GetWaveOrigin() const
{
	const UCapsuleComponent* Capsule = OwnerMiner ? OwnerMiner->GetCapsuleComponent() : nullptr;
	if (Capsule == nullptr)
	{
		return GetShotOrigin();
	}
	// GetShotOrigin is the floor + ShotHeight, pushed ahead of the capsule: bring it back onto the miner's axis.
	const FVector Forward = GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	return GetShotOrigin() - Forward * (Capsule->GetScaledCapsuleRadius() + ShotForwardOffset);
}

ARRArenaGrid* ARRShotgun::ResolveGrid() const
{
	if (OwnerMiner && OwnerMiner->GetArenaGrid())
	{
		return OwnerMiner->GetArenaGrid();
	}
	if (!CachedGrid.IsValid() && GetWorld())
	{
		CachedGrid = RRWorld::FindArenaGrid(GetWorld());
	}
	return CachedGrid.Get();
}

TArray<ARREnemy*> ARRShotgun::FindWaveTargets(const FVector& Origin, const FVector& Direction) const
{
	TArray<ARREnemy*> Targets;
	UWorld* World = GetWorld();
	if (World == nullptr || Range <= 0.f)
	{
		return Targets;
	}
	const FVector Aim = Direction.GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(FMath::Clamp(ConeAngleDeg * 0.5f, 0.f, 180.f)));
	const ARRArenaGrid* Grid = ResolveGrid();

	for (TActorIterator<ARREnemy> It(World); It; ++It)
	{
		ARREnemy* Enemy = *It;
		if (!IsValid(Enemy) || Enemy->IsDead())
		{
			continue;
		}
		FVector ToEnemy = Enemy->GetActorLocation() - Origin;
		ToEnemy.Z = 0.f;
		const float Distance = ToEnemy.Size();
		if (Distance > Range)
		{
			continue;
		}
		// A robot on the apex is inside the cone.
		if (Distance > UE_KINDA_SMALL_NUMBER && FVector::DotProduct(ToEnemy / Distance, Aim) < CosHalfAngle)
		{
			continue;
		}
		if (Grid && !Grid->HasLineOfSight(Origin, Enemy->GetActorLocation()))
		{
			continue;
		}
		Targets.Add(Enemy);
	}

	Targets.Sort([&Origin](const ARREnemy& A, const ARREnemy& B)
	{
		return FVector::DistSquared2D(A.GetActorLocation(), Origin) < FVector::DistSquared2D(B.GetActorLocation(), Origin);
	});
	return Targets;
}

float ARRShotgun::GetWaveClearDistance(float YawOffsetDeg) const
{
	const ARRArenaGrid* Grid = ResolveGrid();
	if (Grid == nullptr)
	{
		return LastWave.Range;
	}
	const FVector Direction = LastWave.Direction.RotateAngleAxis(YawOffsetDeg, FVector::UpVector);
	return Grid->GetClearDistance(LastWave.Origin, Direction, LastWave.Range);
}

void ARRShotgun::FireOnce()
{
	const FVector Origin = GetWaveOrigin();
	const FVector Direction = GetAimDirection().GetSafeNormal2D(UE_SMALL_NUMBER, FVector::ForwardVector);
	const TArray<ARREnemy*> Targets = FindWaveTargets(Origin, Direction);

	// One spec per wave (source attributes snapshot now); each application is executed on its own copy, so each robot rolls its own critical.
	const FGameplayEffectSpecHandle Spec = MakeDamageSpec();
	if (!Spec.IsValid())
	{
		UE_LOG(LogRogueRigs, Warning, TEXT("%s: no damage spec (DamageEffectClass unset); the wave deals no damage."), *GetName());
	}

	// Knockback direction recorded before the damage: a robot killed by this wave is not pushed (ApplyKnockback ignores the dead).
	TArray<TPair<TWeakObjectPtr<ARREnemy>, FVector>> Pushes;
	Pushes.Reserve(Targets.Num());
	for (ARREnemy* Enemy : Targets)
	{
		const FVector Outward = (Enemy->GetActorLocation() - Origin).GetSafeNormal2D(UE_SMALL_NUMBER, Direction);
		Pushes.Emplace(Enemy, Outward);
		if (Spec.IsValid())
		{
			if (UAbilitySystemComponent* TargetASC = Enemy->GetAbilitySystemComponent())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, TargetASC);
			}
		}
	}
	if (KnockbackForce > 0.f)
	{
		for (const TPair<TWeakObjectPtr<ARREnemy>, FVector>& Push : Pushes)
		{
			if (ARREnemy* Enemy = Push.Key.Get())
			{
				Enemy->ApplyKnockback(Push.Value * KnockbackForce);
			}
		}
	}

	const ARRArenaGrid* Grid = ResolveGrid();
	LastWave.Origin = Origin;
	LastWave.Direction = Direction;
	LastWave.HalfAngleDeg = ConeAngleDeg * 0.5f;
	LastWave.Range = Range;
	LastWave.ClippedLength = Grid ? Grid->GetClearDistance(Origin, Direction, Range) : Range;
	LastWave.HitCount = Targets.Num();
	LastWave.Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;
	++WaveCount;

	OnWaveFired(LastWave.Origin, LastWave.Direction, LastWave.HalfAngleDeg, LastWave.Range, LastWave.ClippedLength);
}
