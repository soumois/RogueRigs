// Rogue Rigs - debug log of where a miner shot is drawn on its first frames against the hand weapon Muzzle (task-413, console variable rr.Debug.ShotFirstFrame, off by default).

#include "Debug/RRShotFirstFrameDebug.h"

#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "HAL/IConsoleManager.h"
#include "Projectiles/RRProjectile.h"
#include "RogueRigs.h"

#if !UE_BUILD_SHIPPING

namespace
{
	TAutoConsoleVariable<int32> CVarShotFirstFrame(
		TEXT("rr.Debug.ShotFirstFrame"),
		0,
		TEXT("1 = log each miner shot leaving from the hand weapon Muzzle: Muzzle at fire time, location after the sweep, then on its first 3 frames the shot mesh location against the Muzzle of that frame (task-413). 0 = off."),
		ECVF_Default);

	/** Frames logged per shot, the spawn frame included. */
	constexpr int32 ShotFirstFramesToLog = 3;

	/** One tracked shot. */
	struct FRRShotFirstFrameTrack
	{
		TWeakObjectPtr<ARRProjectile> Projectile;
		TWeakObjectPtr<const USceneComponent> Muzzle;
		TWeakObjectPtr<UWorld> World;
		FVector FireMuzzleLocation = FVector::ZeroVector;
		double SpawnTime = 0.0;
		int32 Id = 0;
		int32 FramesLogged = 0;
	};

	TArray<FRRShotFirstFrameTrack> ShotFirstFrameTracks;
	FDelegateHandle ShotFirstFramePostTickHandle;
	int32 ShotFirstFrameNextId = 1;

	/** The shot mesh: the first primitive other than the collision capsule; the capsule when the projectile has no mesh. */
	const UPrimitiveComponent* FindShotFirstFrameMesh(const ARRProjectile* Projectile)
	{
		TInlineComponentArray<UPrimitiveComponent*> Primitives(Projectile);
		for (const UPrimitiveComponent* Primitive : Primitives)
		{
			if (Primitive != Projectile->GetCollisionCapsule())
			{
				return Primitive;
			}
		}
		return Projectile->GetCollisionCapsule();
	}

	void HandleShotFirstFramePostActorTick(UWorld* World, ELevelTick TickType, float DeltaSeconds)
	{
		for (int32 Index = ShotFirstFrameTracks.Num() - 1; Index >= 0; --Index)
		{
			FRRShotFirstFrameTrack& Shot = ShotFirstFrameTracks[Index];
			if (Shot.World.Get() != World)
			{
				continue;
			}
			ARRProjectile* Projectile = Shot.Projectile.Get();
			if (Projectile == nullptr || Projectile->IsActorBeingDestroyed())
			{
				UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.ShotFirstFrame shot=%d frame=+%d gone"), Shot.Id, Shot.FramesLogged);
				ShotFirstFrameTracks.RemoveAtSwap(Index);
				continue;
			}
			const UPrimitiveComponent* Mesh = FindShotFirstFrameMesh(Projectile);
			const FVector MeshLocation = Mesh ? Mesh->GetComponentLocation() : Projectile->GetActorLocation();
			const USceneComponent* Muzzle = Shot.Muzzle.Get();
			const FVector MuzzleNow = Muzzle ? Muzzle->GetComponentLocation() : Shot.FireMuzzleLocation;
			const bool bRendered = Mesh && Mesh->GetLastRenderTime() >= static_cast<float>(Shot.SpawnTime);
			UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.ShotFirstFrame shot=%d frame=+%d t=%.4f dt=%.4f mesh=%s muzzleNow=%s dist2D_fireMuzzle=%.1f dist2D_muzzleNow=%.1f distZ_muzzleNow=%.1f muzzleMoved2D=%.1f renderedSinceSpawn=%d"),
				Shot.Id, Shot.FramesLogged, World->GetTimeSeconds(), DeltaSeconds, *MeshLocation.ToCompactString(), *MuzzleNow.ToCompactString(),
				FVector::Dist2D(MeshLocation, Shot.FireMuzzleLocation), FVector::Dist2D(MeshLocation, MuzzleNow), MeshLocation.Z - MuzzleNow.Z,
				FVector::Dist2D(MuzzleNow, Shot.FireMuzzleLocation), bRendered ? 1 : 0);
			if (++Shot.FramesLogged >= ShotFirstFramesToLog)
			{
				ShotFirstFrameTracks.RemoveAtSwap(Index);
			}
		}
		if (ShotFirstFrameTracks.IsEmpty() && ShotFirstFramePostTickHandle.IsValid())
		{
			FWorldDelegates::OnWorldPostActorTick.Remove(ShotFirstFramePostTickHandle);
			ShotFirstFramePostTickHandle.Reset();
		}
	}
}

bool RRShotFirstFrameDebug::IsEnabled()
{
	return CVarShotFirstFrame.GetValueOnGameThread() != 0;
}

void RRShotFirstFrameDebug::TrackShot(ARRProjectile* Projectile, const USceneComponent* Muzzle, const FVector& FireMuzzleLocation, const FVector& SpawnLocation, const FVector& SweptLocation)
{
	UWorld* World = Projectile ? Projectile->GetWorld() : nullptr;
	if (!IsEnabled() || World == nullptr)
	{
		return;
	}
	FRRShotFirstFrameTrack Shot;
	Shot.Projectile = Projectile;
	Shot.Muzzle = Muzzle;
	Shot.World = World;
	Shot.FireMuzzleLocation = FireMuzzleLocation;
	Shot.SpawnTime = World->GetTimeSeconds();
	Shot.Id = ShotFirstFrameNextId++;
	UE_LOG(LogRogueRigs, Log, TEXT("rr.Debug.ShotFirstFrame shot=%d fire t=%.4f dt=%.4f inTick=%d class=%s muzzle=%s spawn=%s swept=%s dist2D_swept_fireMuzzle=%.1f"),
		Shot.Id, World->GetTimeSeconds(), World->GetDeltaSeconds(), World->bInTick ? 1 : 0, *GetNameSafe(Projectile->GetClass()),
		*FireMuzzleLocation.ToCompactString(), *SpawnLocation.ToCompactString(), *SweptLocation.ToCompactString(), FVector::Dist2D(SweptLocation, FireMuzzleLocation));
	ShotFirstFrameTracks.Add(Shot);
	if (!ShotFirstFramePostTickHandle.IsValid())
	{
		ShotFirstFramePostTickHandle = FWorldDelegates::OnWorldPostActorTick.AddStatic(&HandleShotFirstFramePostActorTick);
	}
}

#else

bool RRShotFirstFrameDebug::IsEnabled()
{
	return false;
}

void RRShotFirstFrameDebug::TrackShot(ARRProjectile*, const USceneComponent*, const FVector&, const FVector&, const FVector&)
{
}

#endif
