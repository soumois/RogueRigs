// Rogue Rigs - flow field data and algorithms (BFS, incremental relaxation) without any UObject or grid access, so the same code
// runs on the game thread (time-sliced fallback) and on a worker thread over a private copy (task-066-01).

#include "FlowField/RRFlowFieldData.h"

#include "HAL/PlatformTime.h"

const FIntPoint FRRFlowFieldData::Offsets[8] = {
	FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1),
	FIntPoint(1, 1), FIntPoint(1, -1), FIntPoint(-1, 1), FIntPoint(-1, -1)
};

const uint8 FRRFlowFieldData::Opposite[8] = { 1, 0, 3, 2, 7, 6, 5, 4 };

namespace RRFlowFieldData
{
	/** Packs a relaxation heap entry: distance in the high half (heap order), cell index in the low half. */
	uint64 PackEntry(int32 Distance, int32 Index)
	{
		return (static_cast<uint64>(static_cast<uint32>(Distance)) << 32) | static_cast<uint32>(Index);
	}

	/** True when the deadline (FPlatformTime::Seconds, <= 0 for none) has passed. */
	bool IsPastDeadline(double DeadlineSeconds)
	{
		return DeadlineSeconds > 0.0 && FPlatformTime::Seconds() >= DeadlineSeconds;
	}

	/** Squared straight-line distance in cells between two cells (direction tie-break). */
	int32 StraightSquared(FIntPoint From, FIntPoint To)
	{
		const FIntPoint Delta = To - From;
		return Delta.X * Delta.X + Delta.Y * Delta.Y;
	}
}

bool FRRFlowFieldData::CanStep(FIntPoint Cell, int32 Index) const
{
	const FIntPoint Offset = Offsets[Index];
	const FIntPoint Next = Cell + Offset;
	if (!IsInField(Next) || !Walkable[GetIndex(Next)])
	{
		return false;
	}
	if (Offset.X != 0 && Offset.Y != 0)
	{
		// No corner cutting: both orthogonal cells crossed by the diagonal must be walkable.
		return Walkable[GetIndex(FIntPoint(Next.X, Cell.Y))] && Walkable[GetIndex(FIntPoint(Cell.X, Next.Y))];
	}
	return true;
}

uint8 FRRFlowFieldData::ChooseDirection(FIntPoint Cell, const TArray<int32>& InDistances, FIntPoint Goal) const
{
	uint8 Best = NoDirection;
	int32 BestDistance = MAX_int32;
	int32 BestStraight = MAX_int32;
	for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
	{
		if (!CanStep(Cell, Neighbour))
		{
			continue;
		}
		const FIntPoint Next = Cell + Offsets[Neighbour];
		const int32 NextDistance = InDistances[GetIndex(Next)];
		const int32 Straight = RRFlowFieldData::StraightSquared(Next, Goal);
		if (NextDistance != INDEX_NONE && (NextDistance < BestDistance || (NextDistance == BestDistance && Straight < BestStraight)))
		{
			BestDistance = NextDistance;
			BestStraight = Straight;
			Best = static_cast<uint8>(Neighbour);
		}
	}
	return Best;
}

bool FRRFlowFieldData::IsBetterTie(FIntPoint Cell, int32 Candidate, int32 Current, FIntPoint Goal)
{
	const int32 CandidateStraight = RRFlowFieldData::StraightSquared(Cell + Offsets[Candidate], Goal);
	const int32 CurrentStraight = RRFlowFieldData::StraightSquared(Cell + Offsets[Current], Goal);
	return CandidateStraight < CurrentStraight || (CandidateStraight == CurrentStraight && Candidate < Current);
}

bool FRRFlowFieldData::AdvanceBfs(FIntPoint Goal, TArray<int32>& OutDistances, TArray<uint8>& OutDirections, TArray<int32>& Queue,
	int32& Head, int32& DirectionHead, double DeadlineSeconds) const
{
	int32 Steps = 0;
	// Breadth-first from the goal; every step (orthogonal or diagonal) costs 1.
	while (Head < Queue.Num())
	{
		if (++Steps % BudgetCheckInterval == 0 && RRFlowFieldData::IsPastDeadline(DeadlineSeconds))
		{
			return false;
		}
		const int32 Index = Queue[Head++];
		const FIntPoint Cell = GetCell(Index);
		for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
		{
			if (!CanStep(Cell, Neighbour))
			{
				continue;
			}
			const int32 NextIndex = GetIndex(Cell + Offsets[Neighbour]);
			if (OutDistances[NextIndex] == INDEX_NONE)
			{
				OutDistances[NextIndex] = OutDistances[Index] + 1;
				Queue.Add(NextIndex);
			}
		}
	}
	// Directions once every distance is known.
	while (DirectionHead < Queue.Num())
	{
		if (++Steps % BudgetCheckInterval == 0 && RRFlowFieldData::IsPastDeadline(DeadlineSeconds))
		{
			return false;
		}
		const int32 Index = Queue[DirectionHead++];
		if (OutDistances[Index] > 0)
		{
			OutDirections[Index] = ChooseDirection(GetCell(Index), OutDistances, Goal);
		}
	}
	return true;
}

void FRRFlowFieldData::ComputeFull(FIntPoint InGoalCell)
{
	const int32 CellCount = Size.X * Size.Y;
	GoalCell = InGoalCell;
	RelaxHeap.Reset();
	Distances.Init(INDEX_NONE, CellCount);
	Directions.Init(NoDirection, CellCount);
	TArray<int32> Queue;
	Queue.Reserve(CellCount);
	if (IsInField(InGoalCell))
	{
		Distances[GetIndex(InGoalCell)] = 0;
		Queue.Add(GetIndex(InGoalCell));
	}
	int32 Head = 0;
	int32 DirectionHead = 0;
	AdvanceBfs(InGoalCell, Distances, Directions, Queue, Head, DirectionHead, 0.0);
}

void FRRFlowFieldData::SeedAround(FIntPoint Cell)
{
	// The opened cell itself: one step more than its best neighbour, direction set now so robots never meet a gap.
	const int32 Index = GetIndex(Cell);
	const uint8 Best = ChooseDirection(Cell, Distances, GoalCell);
	if (Best != NoDirection && Distances[Index] != 0)
	{
		const int32 Candidate = Distances[GetIndex(Cell + Offsets[Best])] + 1;
		if (Distances[Index] == INDEX_NONE || Candidate < Distances[Index])
		{
			Distances[Index] = Candidate;
			Directions[Index] = Best;
		}
	}
	PushRelax(Index);
	// Its neighbours gain edges (to the cell, and diagonals that no longer cut its corner): relax from each of them.
	for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
	{
		const FIntPoint Next = Cell + Offsets[Neighbour];
		if (IsInField(Next) && Walkable[GetIndex(Next)])
		{
			PushRelax(GetIndex(Next));
		}
	}
}

void FRRFlowFieldData::PushRelax(int32 Index)
{
	if (Distances[Index] != INDEX_NONE)
	{
		RelaxHeap.HeapPush(RRFlowFieldData::PackEntry(Distances[Index], Index));
	}
}

bool FRRFlowFieldData::RunRelaxation(double DeadlineSeconds)
{
	int32 Steps = 0;
	while (RelaxHeap.Num() > 0)
	{
		if (++Steps % BudgetCheckInterval == 0 && RRFlowFieldData::IsPastDeadline(DeadlineSeconds))
		{
			return false;
		}
		uint64 Entry = 0;
		RelaxHeap.HeapPop(Entry, EAllowShrinking::No);
		const int32 Index = static_cast<int32>(Entry & 0xFFFFFFFFull);
		const int32 Distance = static_cast<int32>(Entry >> 32);
		if (Distances[Index] != Distance)
		{
			continue; // Stale: the cell got shorter since this entry was pushed.
		}
		const FIntPoint Cell = GetCell(Index);
		if (Distance > 0)
		{
			// Lowest distance first: the neighbours closer than this cell are final, so the full rule applies.
			Directions[Index] = ChooseDirection(Cell, Distances, GoalCell);
		}
		for (int32 Neighbour = 0; Neighbour < 8; ++Neighbour)
		{
			if (!CanStep(Cell, Neighbour))
			{
				continue;
			}
			const FIntPoint Next = Cell + Offsets[Neighbour];
			const int32 NextIndex = GetIndex(Next);
			const int32 NextDistance = Distances[NextIndex];
			const uint8 Back = Opposite[Neighbour];
			if (NextDistance == INDEX_NONE || Distance + 1 < NextDistance)
			{
				// Shorter through this cell: valid direction now, refined when Next is popped.
				Distances[NextIndex] = Distance + 1;
				Directions[NextIndex] = Back;
				PushRelax(NextIndex);
			}
			else if (Distance + 1 == NextDistance
				&& (Directions[NextIndex] == NoDirection || IsBetterTie(Next, Back, Directions[NextIndex], GoalCell)))
			{
				// Same distance through this cell, better tie-break: what a full rebuild would pick.
				Directions[NextIndex] = Back;
			}
		}
	}
	return true;
}

void FRRFlowFieldJob::Run()
{
	const double StartSeconds = FPlatformTime::Seconds();
	if (bGoalRebuild)
	{
		// The openings are already in Field.Walkable: the BFS includes them.
		Field.ComputeFull(RebuildGoalCell);
	}
	else
	{
		for (const int32 Index : OpenedIndices)
		{
			Field.SeedAround(Field.GetCell(Index));
		}
		Field.RunRelaxation(0.0);
	}
	WorkerMilliseconds = (FPlatformTime::Seconds() - StartSeconds) * 1000.0;
}
