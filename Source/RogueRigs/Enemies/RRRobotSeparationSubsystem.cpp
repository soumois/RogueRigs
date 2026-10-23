// Rogue Rigs - robot separation world subsystem: a cheap push between overlapping robots, replacing RVO avoidance (task-034-01).

#include "Enemies/RRRobotSeparationSubsystem.h"

#include "Enemies/RREnemy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RRRobotSeparationSubsystem)

namespace
{
	FIntPoint BucketOf(const FVector2D& Position)
	{
		return FIntPoint(FMath::FloorToInt32(Position.X / URRRobotSeparationSubsystem::BucketSize),
			FMath::FloorToInt32(Position.Y / URRRobotSeparationSubsystem::BucketSize));
	}
}

void URRRobotSeparationSubsystem::RegisterRobot(ARREnemy* Robot)
{
	if (Robot)
	{
		Robots.AddUnique(Robot);
		BuiltFrame = MAX_uint64;
	}
}

void URRRobotSeparationSubsystem::UnregisterRobot(ARREnemy* Robot)
{
	Robots.RemoveSwap(Robot, EAllowShrinking::No);
	BuiltFrame = MAX_uint64;
}

void URRRobotSeparationSubsystem::RebuildIfNeeded()
{
	if (BuiltFrame == GFrameCounter)
	{
		return;
	}
	BuiltFrame = GFrameCounter;
	Positions.Reset();
	Radii.Reset();
	Entries.Reset();
	Buckets.Reset();

	for (int32 Index = Robots.Num() - 1; Index >= 0; --Index)
	{
		const ARREnemy* Robot = Robots[Index].Get();
		if (!Robot)
		{
			Robots.RemoveAtSwap(Index, 1, EAllowShrinking::No);
			continue;
		}
		if (Robot->IsDead())
		{
			continue;
		}
		const FVector2D Position(Robot->GetActorLocation());
		const int32 Entry = Entries.Add(Robot);
		Positions.Add(Position);
		const float SeparationRadius = Robot->GetSeparationRadius();
		// The 3 x 3 bucket read only finds every contact while two separation radii fit in one bucket.
		ensureMsgf(2.f * SeparationRadius <= BucketSize, TEXT("%s separation radius %.1f exceeds half the bucket size %.0f"),
			*Robot->GetName(), SeparationRadius, BucketSize);
		Radii.Add(SeparationRadius);
		Buckets.FindOrAdd(BucketOf(Position)).Add(Entry);
	}
}

FVector URRRobotSeparationSubsystem::GetSeparation(const ARREnemy* Robot)
{
	if (!Robot)
	{
		return FVector::ZeroVector;
	}
	RebuildIfNeeded();

	const FVector2D Position(Robot->GetActorLocation());
	const float Radius = Robot->GetSeparationRadius();
	const FIntPoint Center = BucketOf(Position);
	FVector2D Push = FVector2D::ZeroVector;
	for (int32 Y = Center.Y - 1; Y <= Center.Y + 1; ++Y)
	{
		for (int32 X = Center.X - 1; X <= Center.X + 1; ++X)
		{
			const TArray<int32>* Bucket = Buckets.Find(FIntPoint(X, Y));
			if (!Bucket)
			{
				continue;
			}
			for (const int32 Entry : *Bucket)
			{
				if (Entries[Entry] == Robot)
				{
					continue;
				}
				const float Contact = Radius + Radii[Entry];
				const FVector2D Away = Position - Positions[Entry];
				const double DistanceSquared = Away.SizeSquared();
				if (Contact <= 0.f || DistanceSquared >= FMath::Square(Contact))
				{
					continue;
				}
				const double Distance = FMath::Sqrt(DistanceSquared);
				// Stacked exactly: split along a direction fixed by the pair order, so both robots part.
				const FVector2D Direction = Distance > UE_KINDA_SMALL_NUMBER ? Away / Distance
					: (Robot < Entries[Entry] ? FVector2D(1.0, 0.0) : FVector2D(-1.0, 0.0));
				Push += Direction * ((Contact - Distance) / Contact);
			}
		}
	}
	const double Length = Push.Size();
	if (Length > 1.0)
	{
		Push /= Length;
	}
	return FVector(Push.X, Push.Y, 0.0);
}
