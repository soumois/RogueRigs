// Rogue Rigs - light aim assist of the miner's shots (task-463): predicts what a shot hits first and turns it a few degrees toward a robot.

#include "Weapons/RRAimAssist.h"

#include "Components/CapsuleComponent.h"
#include "Enemies/RREnemy.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "Grid/RRArenaGrid.h"
#include "Projectiles/RRCollisionChannels.h"

namespace RRAimAssist
{
	namespace
	{
		/** Living robot whose hit capsule is HitComponent (the shape the shots hit), else null. */
		const ARREnemy* AsLivingRobot(const AActor* Actor, const UPrimitiveComponent* HitComponent)
		{
			const ARREnemy* Robot = Cast<ARREnemy>(Actor);
			if (!IsValid(Robot) || Robot->IsDead() || Robot->IsDeathPending() || HitComponent == nullptr || HitComponent != Robot->GetHitCapsule())
			{
				return nullptr;
			}
			return Robot;
		}

		/** First relevant hit of one capsule sweep from Start to End (Nothing when it meets nothing that counts). */
		EFirstHit SweepFirstHit(const UWorld& World, const FShotQuery& Query, const FVector& Start, const FVector& End, const ARREnemy** OutRobot)
		{
			const FCollisionShape Shape = FCollisionShape::MakeCapsule(Query.Radius, FMath::Max(Query.HalfHeight, Query.Radius));
			FCollisionQueryParams Params(SCENE_QUERY_STAT(RRAimAssist), /*bTraceComplex*/ false);
			for (const AActor* Ignored : Query.IgnoredActors)
			{
				Params.AddIgnoredActor(Ignored);
			}

			TArray<FHitResult> Hits;
			World.SweepMultiByChannel(Hits, Start, End, FQuat::Identity, RRCollisionChannels::Projectile, Shape, Params, Query.Responses);
			// Touches (robots) come before the blocking hit; sorted again so a start-penetrating hit (distance 0) is seen first.
			Hits.StableSort([](const FHitResult& A, const FHitResult& B) { return A.Distance < B.Distance; });

			for (const FHitResult& Hit : Hits)
			{
				if (const ARREnemy* Robot = AsLivingRobot(Hit.GetActor(), Hit.GetComponent()))
				{
					if (OutRobot)
					{
						*OutRobot = Robot;
					}
					return EFirstHit::Robot;
				}
				if (!Hit.bBlockingHit)
				{
					continue;
				}
				if (const ARRArenaGrid* Grid = Cast<ARRArenaGrid>(Hit.GetActor()))
				{
					const FIntPoint Cell = Grid->GetCellFromHit(Hit);
					if (Grid->GetCellType(Cell) == ERRCellType::Barrel)
					{
						return EFirstHit::Barrel;
					}
					if (!Grid->IsBlockCell(Cell))
					{
						// The floor slab under the capsule bottom (start penetration): not on the shot's way.
						continue;
					}
				}
				return EFirstHit::Block;
			}
			return EFirstHit::Nothing;
		}

		/** A robot within the cone, with the yaw turn it needs. */
		struct FCandidate
		{
			const ARREnemy* Robot = nullptr;
			float AngleDeg = 0.f;
			FVector Direction = FVector::ZeroVector;
		};
	}

	EFirstHit PredictFirstHit(const UWorld& World, const FShotQuery& Query, const FVector& Direction, const ARREnemy** OutRobot)
	{
		if (Query.bSweepFromMiner)
		{
			const EFirstHit First = SweepFirstHit(World, Query, Query.SweepStart, Query.Origin, OutRobot);
			if (First != EFirstHit::Nothing)
			{
				return First;
			}
		}
		const FVector Flat = Direction.GetSafeNormal2D();
		if (Flat.IsNearlyZero() || Query.Range <= 0.f)
		{
			return EFirstHit::Nothing;
		}
		return SweepFirstHit(World, Query, Query.Origin, Query.Origin + Flat * Query.Range, OutRobot);
	}

	bool FindAssistedDirection(const UWorld& World, const FShotQuery& Query, const FVector& ShotDirection, float MaxAngleDeg,
		FVector& OutDirection, const ARREnemy** OutTarget)
	{
		OutDirection = ShotDirection;
		const FVector Shot2D = ShotDirection.GetSafeNormal2D();
		if (MaxAngleDeg <= 0.f || Query.Range <= 0.f || Shot2D.IsNearlyZero())
		{
			return false;
		}

		// Q42: the shot as fired (spread included, real width) meets a robot or a barrel first: left alone.
		const EFirstHit Straight = PredictFirstHit(World, Query, Shot2D);
		if (Straight == EFirstHit::Robot || Straight == EFirstHit::Barrel)
		{
			return false;
		}

		// Q41: robots within Range and the cone, smallest turn first.
		TArray<FOverlapResult> Overlaps;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(RRAimAssistCandidates), /*bTraceComplex*/ false);
		World.OverlapMultiByObjectType(Overlaps, Query.Origin, FQuat::Identity, FCollisionObjectQueryParams(ECC_Pawn), FCollisionShape::MakeSphere(Query.Range), Params);

		const double CosMax = FMath::Cos(FMath::DegreesToRadians(static_cast<double>(MaxAngleDeg)));
		TArray<FCandidate, TInlineAllocator<8>> Candidates;
		for (const FOverlapResult& Overlap : Overlaps)
		{
			const ARREnemy* Robot = AsLivingRobot(Overlap.GetActor(), Overlap.GetComponent());
			if (Robot == nullptr)
			{
				continue;
			}
			FVector ToRobot = Robot->GetHitCapsule()->GetComponentLocation() - Query.Origin;
			ToRobot.Z = 0.0;
			const double Distance = ToRobot.Size();
			if (Distance <= UE_KINDA_SMALL_NUMBER || Distance > Query.Range)
			{
				continue;
			}
			const FVector Direction = ToRobot / Distance;
			const double Cos = FVector::DotProduct(Direction, Shot2D);
			if (Cos < CosMax)
			{
				continue;
			}
			Candidates.Add({ Robot, static_cast<float>(FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Cos, -1.0, 1.0)))), Direction });
		}
		Candidates.Sort([](const FCandidate& A, const FCandidate& B) { return A.AngleDeg < B.AngleDeg; });

		// The turned shot must meet a robot first: a rock block or a barrel in the way (real shot width) rules the robot out.
		for (const FCandidate& Candidate : Candidates)
		{
			const ARREnemy* FirstRobot = nullptr;
			if (PredictFirstHit(World, Query, Candidate.Direction, &FirstRobot) == EFirstHit::Robot)
			{
				OutDirection = Candidate.Direction;
				if (OutTarget)
				{
					*OutTarget = Candidate.Robot;
				}
				return true;
			}
		}
		return false;
	}
}
