// Rogue Rigs - deterministic 240 x 240 layout generator (C++ port of Tools/layout_gen/arena_gen_fine.js).

#include "Grid/RRLayoutGenerator.h"

#include "HAL/PlatformTime.h"
#include "Misc/ScopeExit.h"
#include "RogueRigs.h"

namespace
{
	const FIntPoint D4[4] = { FIntPoint(1, 0), FIntPoint(-1, 0), FIntPoint(0, 1), FIntPoint(0, -1) };
	constexpr int32 FarDistance = 1 << 20;
	constexpr double TwoPiApprox = 6.283;

	/** JavaScript Math.round: nearest integer, halves toward +infinity. */
	int32 RoundHalfUp(double Value)
	{
		return FMath::FloorToInt32(Value + 0.5);
	}

	double Hypot(double X, double Y)
	{
		return FMath::Sqrt(X * X + Y * Y);
	}
}

FRRLayoutGenerator::FRRLayoutGenerator(const FRRLayoutGenParams& InParams)
	: P(InParams)
{
}

double FRRLayoutGenerator::Rnd()
{
	return static_cast<double>(Stream.GetFraction());
}

double FRRLayoutGenerator::Gauss()
{
	const double A = Rnd();
	const double B = Rnd();
	const double C = Rnd();
	return (A + B + C - 1.5) * 1.15;
}

double FRRLayoutGenerator::Hash(int32 X, int32 Y, int32 K) const
{
	uint32 Value = static_cast<uint32>(X) * 374761393u + static_cast<uint32>(Y) * 668265263u + static_cast<uint32>(K) * 2147483647u
		+ static_cast<uint32>(NoiseSeedMixed);
	Value = (Value ^ (Value >> 13)) * 1274126177u;
	Value ^= Value >> 16;
	return static_cast<double>(Value) / 4294967296.0;
}

double FRRLayoutGenerator::ValueNoise(double X, double Y, double Scale, int32 K) const
{
	const double FX = X / Scale;
	const double FY = Y / Scale;
	const int32 X0 = FMath::FloorToInt32(FX);
	const int32 Y0 = FMath::FloorToInt32(FY);
	const double TX = FX - X0;
	const double TY = FY - Y0;
	const double SmoothX = TX * TX * (3.0 - 2.0 * TX);
	const double SmoothY = TY * TY * (3.0 - 2.0 * TY);
	const double A = Hash(X0, Y0, K);
	const double B = Hash(X0 + 1, Y0, K);
	const double C = Hash(X0, Y0 + 1, K);
	const double D = Hash(X0 + 1, Y0 + 1, K);
	return (A + (B - A) * SmoothX) * (1.0 - SmoothY) + (C + (D - C) * SmoothX) * SmoothY;
}

TArray<FRRLayoutGenerator::FOutlineRun> FRRLayoutGenerator::FindOutlineRuns(int32 MaxRun) const
{
	TArray<FOutlineRun> Result;
	for (const FIntPoint& Dir : D4)
	{
		const bool bVertical = Dir.X != 0;
		const int32 Along = bVertical ? H : W;
		const int32 Across = bVertical ? W : H;
		for (int32 A = 0; A < Across; ++A)
		{
			TArray<FIntPoint> Run;
			for (int32 B = 0; B <= Along; ++B)
			{
				const int32 X = bVertical ? A : B;
				const int32 Y = bVertical ? B : A;
				if (B < Along && IsAsteroid(X, Y) && !IsAsteroid(X + Dir.X, Y + Dir.Y))
				{
					Run.Add(FIntPoint(X, Y));
				}
				else
				{
					if (Run.Num() > MaxRun)
					{
						Result.Add(FOutlineRun{ MoveTemp(Run), Dir });
					}
					Run.Reset();
				}
			}
		}
	}
	return Result;
}

void FRRLayoutGenerator::MarkHidden(const TArray<int32>& Cells)
{
	const int32 Guard = P.Pockets.Guard;
	for (const int32 I : Cells)
	{
		Hidden[I] = 1;
		const int32 X = I % W;
		const int32 Y = I / W;
		for (int32 J = -Guard; J <= Guard; ++J)
		{
			for (int32 K = -Guard; K <= Guard; ++K)
			{
				if (InBounds(X + K, Y + J))
				{
					HiddenGuard[(Y + J) * W + X + K] = 1;
				}
			}
		}
	}
}

TArray<int32> FRRLayoutGenerator::WanderWalk(double X, double Y, double Angle, int32 Len, double Rho0, double Turn)
{
	const FRRLayoutGenWalkParams& WK = P.Walk;
	++WalkId;
	TArray<int32> Cells;
	double Rho = Rho0;
	const double Amp = WK.BrushNoiseAmp;
	for (int32 Step = 0; Step < Len; ++Step)
	{
		Rho = FMath::Clamp(Rho + Gauss() * WK.BrushDrift, WK.BrushMin, WK.BrushMax);
		const int32 R = FMath::CeilToInt32(Rho + Amp) + 1;
		const int32 RX = RoundHalfUp(X);
		const int32 RY = RoundHalfUp(Y);
		for (int32 J = -R; J <= R; ++J)
		{
			for (int32 I = -R; I <= R; ++I)
			{
				const int32 CX = RX + I;
				const int32 CY = RY + J;
				if (!IsAsteroid(CX, CY))
				{
					continue;
				}
				const int32 Id = CY * W + CX;
				if (Stamp[Id] == WalkId)
				{
					continue;
				}
				if (Hypot(CX - X, CY - Y) <= Rho + Amp * (2.0 * ValueNoise(CX, CY, WK.BrushNoiseScale, 2) - 1.0))
				{
					Stamp[Id] = WalkId;
					Cells.Add(Id);
				}
			}
		}
		Angle += Gauss() * Turn;
		if (Rnd() < WK.SharpChance)
		{
			const double Sign = Rnd() < 0.5 ? 1.0 : -1.0;
			Angle += Sign * (WK.SharpMin + Rnd() * (WK.SharpMax - WK.SharpMin));
		}
		X += FMath::Cos(Angle) * WK.Step;
		Y += FMath::Sin(Angle) * WK.Step;
		const int32 NX = RoundHalfUp(X);
		const int32 NY = RoundHalfUp(Y);
		if (!InBounds(NX, NY) || Safe[NY * W + NX] != 1)
		{
			break;
		}
	}
	Cells.RemoveAll([this](int32 I) { return Safe[I] == 0 || HiddenGuard[I] != 0; });
	return Cells;
}

void FRRLayoutGenerator::ApplyCarve(const TArray<int32>& Cells)
{
	for (const int32 I : Cells)
	{
		if (Grid[I] == ERRCellType::Rock)
		{
			Grid[I] = ERRCellType::Floor;
			++FloorCount;
		}
	}
}

TArray<uint8> FRRLayoutGenerator::Reach(int32* OutCount) const
{
	TArray<uint8> Mask;
	TArray<int32> Stack;
	ReachInto(Mask, Stack, OutCount);
	return Mask;
}

void FRRLayoutGenerator::ReachInto(TArray<uint8>& Mask, TArray<int32>& Stack, int32* OutCount) const
{
	Mask.SetNumUninitialized(N);
	FMemory::Memzero(Mask.GetData(), N);
	Stack.Reset();
	const int32 Start = SY * W + SX;
	Mask[Start] = 1;
	Stack.Add(Start);
	int32 Count = 1;
	while (Stack.Num() > 0)
	{
		const int32 I = Stack.Pop(EAllowShrinking::No);
		const int32 X = I % W;
		const int32 Y = I / W;
		for (const FIntPoint& D : D4)
		{
			const int32 NX = X + D.X;
			const int32 NY = Y + D.Y;
			if (!InBounds(NX, NY))
			{
				continue;
			}
			const int32 J = NY * W + NX;
			if (!Mask[J] && IsWalkable(Grid[J]))
			{
				Mask[J] = 1;
				++Count;
				Stack.Add(J);
			}
		}
	}
	if (OutCount)
	{
		*OutCount = Count;
	}
}

TArray<int32> FRRLayoutGenerator::Bfs(const TArray<uint8>& Source) const
{
	TArray<int32> Dist;
	TArray<int32> Queue;
	BfsInto(Source, Dist, Queue);
	return Dist;
}

void FRRLayoutGenerator::BfsInto(const TArray<uint8>& Source, TArray<int32>& Dist, TArray<int32>& Queue) const
{
	Dist.Init(FarDistance, N);
	Queue.SetNumUninitialized(N);
	int32 Head = 0;
	int32 Tail = 0;
	for (int32 I = 0; I < N; ++I)
	{
		if (Source[I])
		{
			Dist[I] = 0;
			Queue[Tail++] = I;
		}
	}
	while (Head < Tail)
	{
		const int32 I = Queue[Head++];
		const int32 X = I % W;
		const int32 Y = I / W;
		for (const FIntPoint& D : D4)
		{
			const int32 NX = X + D.X;
			const int32 NY = Y + D.Y;
			if (!IsAsteroid(NX, NY))
			{
				continue;
			}
			const int32 J = NY * W + NX;
			if (Dist[J] > Dist[I] + 1)
			{
				Dist[J] = Dist[I] + 1;
				Queue[Tail++] = J;
			}
		}
	}
}

TArray<int32> FRRLayoutGenerator::BfsFromWalkable() const
{
	TArray<uint8> Source;
	Source.SetNumZeroed(N);
	for (int32 I = 0; I < N; ++I)
	{
		Source[I] = IsWalkable(Grid[I]) ? 1 : 0;
	}
	return Bfs(Source);
}

TArray<FRRLayoutGenerator::FRegion> FRRLayoutGenerator::FindRegions(TArray<uint8>* OutReach, int32* OutReachCount) const
{
	int32 ReachCount = 0;
	const TArray<uint8> ReachMask = Reach(&ReachCount);
	const TArray<int32> Dist = Bfs(ReachMask);
	TArray<int32> Label;
	Label.Init(-1, N);
	TArray<FRegion> Regions;
	for (int32 I0 = 0; I0 < N; ++I0)
	{
		if (!IsWalkable(Grid[I0]) || ReachMask[I0] || Label[I0] >= 0)
		{
			continue;
		}
		FRegion Region;
		Region.Cells.Add(I0);
		Label[I0] = Regions.Num();
		int32 MinDist = FarDistance;
		for (int32 Head = 0; Head < Region.Cells.Num(); ++Head)
		{
			const int32 I = Region.Cells[Head];
			const int32 X = I % W;
			const int32 Y = I / W;
			MinDist = FMath::Min(MinDist, Dist[I]);
			for (const FIntPoint& D : D4)
			{
				const int32 NX = X + D.X;
				const int32 NY = Y + D.Y;
				if (!InBounds(NX, NY))
				{
					continue;
				}
				const int32 J = NY * W + NX;
				if (IsWalkable(Grid[J]) && !ReachMask[J] && Label[J] < 0)
				{
					Label[J] = Regions.Num();
					Region.Cells.Add(J);
				}
			}
		}
		Region.Gap = MinDist - 1;
		Regions.Add(MoveTemp(Region));
	}
	if (OutReach)
	{
		*OutReach = ReachMask;
	}
	if (OutReachCount)
	{
		*OutReachCount = ReachCount;
	}
	return Regions;
}

int32 FRRLayoutGenerator::RunFiller()
{
	const double StartSeconds = FPlatformTime::Seconds();
	ON_SCOPE_EXIT { FillerSeconds += FPlatformTime::Seconds() - StartSeconds; };
	const FRRLayoutGenFillerParams& FL = P.Filler;
	// Scratch buffers allocated once per call. The distance field, the far list and the reachable cells only change when a
	// walk carves a cell (ApplyCarve only turns rock into floor and counts it), so they are rebuilt only then; the random
	// draws stay the same in number and order.
	TArray<uint8> Source;
	TArray<int32> Dist;
	TArray<int32> Queue;
	TArray<uint8> ReachMask;
	TArray<int32> Stack;
	TArray<int32> Far;
	TArray<int32> Reachable;
	Source.SetNumUninitialized(N);
	bool bDirty = true;
	int32 Iteration = 0;
	for (; Iteration < FL.MaxIter; ++Iteration)
	{
		if (bDirty)
		{
			for (int32 I = 0; I < N; ++I)
			{
				Source[I] = IsWalkable(Grid[I]) ? 1 : 0;
			}
			BfsInto(Source, Dist, Queue);
			Far.Reset();
			for (int32 I = 0; I < N; ++I)
			{
				if (Grid[I] != ERRCellType::Void && Dist[I] > FL.MaxD)
				{
					Far.Add(I);
				}
			}
			if (Far.Num() > 0)
			{
				ReachInto(ReachMask, Stack, nullptr);
				Reachable.Reset();
				for (int32 I = 0; I < N; ++I)
				{
					if (ReachMask[I])
					{
						Reachable.Add(I);
					}
				}
			}
			bDirty = false;
		}
		if (Far.Num() == 0)
		{
			break;
		}
		const int32 Target = Far[FMath::FloorToInt32(Rnd() * Far.Num())];
		const int32 TX = Target % W;
		const int32 TY = Target / W;
		// Nearest reachable cell, lowest index on a tie: squared integer distances keep the strict order of the distances
		// (distinct integers up to 2 x 240^2 have distinct square roots), and one square root gives the same BestDist.
		int32 Best = -1;
		int32 BestD2 = MAX_int32;
		for (const int32 I : Reachable)
		{
			const int32 DX = I % W - TX;
			const int32 DY = I / W - TY;
			const int32 D2 = DX * DX + DY * DY;
			if (D2 < BestD2)
			{
				BestD2 = D2;
				Best = I;
			}
		}
		const double BestDist = Best >= 0 ? FMath::Sqrt(static_cast<double>(BestD2)) : 1e9;
		const int32 BX = Best % W;
		const int32 BY = Best / W;
		const double Angle = FMath::Atan2(static_cast<double>(TY - BY), static_cast<double>(TX - BX)) + Gauss() * FL.AimNoise;
		const int32 Len = FMath::CeilToInt32(BestDist) + FL.ExtraMin + FMath::FloorToInt32(Rnd() * FL.ExtraRand);
		const double Rho0 = FL.Rho0Min + Rnd() * (FL.Rho0Max - FL.Rho0Min);
		const int32 FloorBefore = FloorCount;
		ApplyCarve(WanderWalk(BX, BY, Angle, Len, Rho0, P.Walk.Turn * FL.TurnScale));
		bDirty = FloorCount != FloorBefore;
		++FillerIterations;
		FillerIdleIterations += bDirty ? 0 : 1;
	}
	return Iteration;
}

void FRRLayoutGenerator::RefillRegions()
{
	const FRRLayoutGenCleanupParams& CL = P.Cleanup;
	for (const FRegion& Region : FindRegions())
	{
		if (Region.Cells.Num() < CL.MinRegion || Region.Gap > CL.MaxGap)
		{
			for (const int32 I : Region.Cells)
			{
				Grid[I] = ERRCellType::Rock;
				Hidden[I] = 0;
				--FloorCount;
			}
		}
	}
}

void FRRLayoutGenerator::FillRockIslands()
{
	// Full flood of each rock component: one that touches no void and has MaxRockIsland cells or fewer becomes floor.
	const int32 MaxIsland = P.Cleanup.MaxRockIsland;
	TArray<uint8> Label;
	Label.SetNumZeroed(N);
	TArray<int32> Cells;
	for (int32 I0 = 0; I0 < N; ++I0)
	{
		if (Grid[I0] != ERRCellType::Rock || Label[I0])
		{
			continue;
		}
		Cells.Reset();
		Cells.Add(I0);
		Label[I0] = 1;
		bool bTouchesVoid = false;
		for (int32 Head = 0; Head < Cells.Num(); ++Head)
		{
			const int32 I = Cells[Head];
			const int32 X = I % W;
			const int32 Y = I / W;
			for (const FIntPoint& D : D4)
			{
				const int32 NX = X + D.X;
				const int32 NY = Y + D.Y;
				if (!IsAsteroid(NX, NY))
				{
					bTouchesVoid = true;
					continue;
				}
				const int32 J = NY * W + NX;
				if (Grid[J] == ERRCellType::Rock && !Label[J])
				{
					Label[J] = 1;
					Cells.Add(J);
				}
			}
		}
		if (!bTouchesVoid && Cells.Num() <= MaxIsland)
		{
			for (const int32 I : Cells)
			{
				Grid[I] = ERRCellType::Floor;
				++FloorCount;
			}
		}
	}
}

TArray<uint8> FRRLayoutGenerator::CoveredMap() const
{
	const int32 K = P.Cleanup.Passage;
	const int32 W1 = W + 1;
	TArray<int32> Sat;
	Sat.SetNumZeroed(W1 * (H + 1));
	for (int32 Y = 0; Y < H; ++Y)
	{
		for (int32 X = 0; X < W; ++X)
		{
			Sat[(Y + 1) * W1 + X + 1] = Sat[Y * W1 + X + 1] + Sat[(Y + 1) * W1 + X] - Sat[Y * W1 + X] + (IsOpen(Grid[Y * W + X]) ? 1 : 0);
		}
	}
	TArray<uint8> Covered;
	Covered.SetNumZeroed(N);
	for (int32 Y = 0; Y + K <= H; ++Y)
	{
		for (int32 X = 0; X + K <= W; ++X)
		{
			if (Sat[(Y + K) * W1 + X + K] - Sat[Y * W1 + X + K] - Sat[(Y + K) * W1 + X] + Sat[Y * W1 + X] == K * K)
			{
				for (int32 DY = 0; DY < K; ++DY)
				{
					for (int32 DX = 0; DX < K; ++DX)
					{
						Covered[(Y + DY) * W + X + DX] = 1;
					}
				}
			}
		}
	}
	return Covered;
}

int32 FRRLayoutGenerator::PassageRepair()
{
	int32 Changed = 0;
	for (int32 Round = 0; Round < P.Cleanup.RepairRounds; ++Round)
	{
		const TArray<uint8> Covered = CoveredMap();
		int32 Count = 0;
		for (int32 I = 0; I < N; ++I)
		{
			if (Grid[I] == ERRCellType::Floor && !Covered[I])
			{
				Grid[I] = ERRCellType::Rock;
				Hidden[I] = 0;
				--FloorCount;
				++Count;
			}
		}
		Changed += Count;
		if (Count == 0)
		{
			break;
		}
	}
	return Changed;
}

void FRRLayoutGenerator::TrimFar()
{
	const TArray<int32> Dist = BfsFromWalkable();
	for (int32 I = 0; I < N; ++I)
	{
		if (Grid[I] == ERRCellType::Rock && Dist[I] > P.Filler.MaxD && !Safe[I])
		{
			Grid[I] = ERRCellType::Void;
			--Total;
		}
	}
}

int32 FRRLayoutGenerator::CountNarrowCells() const
{
	const TArray<uint8> Covered = CoveredMap();
	int32 Count = 0;
	for (int32 I = 0; I < N; ++I)
	{
		if (IsOpen(Grid[I]) && !Covered[I])
		{
			++Count;
		}
	}
	return Count;
}

uint32 FRRLayoutGenerator::HashGrid() const
{
	return HashCells(Grid);
}

uint32 FRRLayoutGenerator::HashCells(const TArray<ERRCellType>& Cells)
{
	uint32 Value = 2166136261u;
	for (const ERRCellType Type : Cells)
	{
		Value = (Value ^ static_cast<uint32>(Type)) * 16777619u;
	}
	return Value;
}

void FRRLayoutGenerator::Carve(int32 Seed)
{
	W = P.Width;
	H = P.Height;
	N = W * H;
	SX = W >> 1;
	SY = H >> 1;
	const double CX = (W - 1) / 2.0;
	const double CY = (H - 1) / 2.0;
	Stream.Initialize(Seed);
	NoiseSeedMixed = static_cast<int32>(static_cast<uint32>(Seed) * 7919u + static_cast<uint32>(P.NoiseSeed));
	Grid.Init(ERRCellType::Void, N);
	Safe.Init(0, N);
	Hidden.Init(0, N);
	HiddenGuard.Init(0, N);
	Stamp.Init(0, N);
	WalkId = 0;
	Total = 0;
	FloorCount = 0;
	PocketCount = 0;
	RepairCarves = 0;
	RepairOutcrops = 0;
	RepairLanes = 0;
	FillerSeconds = 0.0;
	FillerIterations = 0;
	FillerIdleIterations = 0;

	// 1. Outline: polar radius plus value-noise jitter, clamped EdgeMargin cells from the grid edge.
	const FRRLayoutGenOutlineParams& O = P.Outline;
	const int32 EM = O.EdgeMargin;
	for (int32 Y = 0; Y < H; ++Y)
	{
		for (int32 X = 0; X < W; ++X)
		{
			const double DX = X - CX;
			const double DY = Y - CY;
			const double Theta = FMath::Atan2(DY, DX);
			const double Dist = Hypot(DX, DY);
			double Radius = O.Base;
			for (const FVector& Harmonic : O.Harmonics)
			{
				Radius += Harmonic.X * FMath::Sin(Harmonic.Y * Theta + Harmonic.Z);
			}
			Radius += O.NoiseAmp * (2.0 * ValueNoise(X, Y, O.NoiseScale, 1) - 1.0);
			const bool bInside = Dist <= Radius && X >= EM && X < W - EM && Y >= EM && Y < H - EM;
			Grid[Y * W + X] = bInside ? ERRCellType::Rock : ERRCellType::Void;
		}
	}

	// Notch straight outline runs longer than MaxRun.
	const FRRLayoutGenNotchParams& NT = P.Notch;
	for (int32 Iteration = 0; Iteration < NT.MaxIter; ++Iteration)
	{
		const TArray<FOutlineRun> Runs = FindOutlineRuns(NT.MaxRun);
		if (Runs.Num() == 0)
		{
			break;
		}
		const FOutlineRun& Run = Runs[FMath::FloorToInt32(Rnd() * Runs.Num())];
		const int32 RunLen = Run.Cells.Num();
		const int32 Mid = RunLen / 2 + FMath::FloorToInt32((Rnd() - 0.5) * (RunLen - NT.LenMax));
		const int32 NotchLen = NT.LenMin + FMath::FloorToInt32(Rnd() * (NT.LenMax - NT.LenMin + 1));
		for (int32 K = 0; K < NotchLen; ++K)
		{
			const FIntPoint& C = Run.Cells[FMath::Clamp(Mid + K, 0, RunLen - 1)];
			for (int32 Depth = 0; Depth < NT.Depth; ++Depth)
			{
				const int32 X = C.X - Run.Dir.X * Depth;
				const int32 Y = C.Y - Run.Dir.Y * Depth;
				if (InBounds(X, Y))
				{
					Grid[Y * W + X] = ERRCellType::Void;
				}
			}
		}
	}

	// Drop any fragment cut off by notching (keep the piece holding the spawn).
	{
		TArray<uint8> Seen;
		Seen.SetNumZeroed(N);
		TArray<int32> Stack;
		Stack.Add(SY * W + SX);
		Seen[SY * W + SX] = 1;
		while (Stack.Num() > 0)
		{
			const int32 I = Stack.Pop(EAllowShrinking::No);
			const int32 X = I % W;
			const int32 Y = I / W;
			for (const FIntPoint& D : D4)
			{
				const int32 NX = X + D.X;
				const int32 NY = Y + D.Y;
				if (!IsAsteroid(NX, NY))
				{
					continue;
				}
				const int32 J = NY * W + NX;
				if (!Seen[J])
				{
					Seen[J] = 1;
					Stack.Add(J);
				}
			}
		}
		for (int32 I = 0; I < N; ++I)
		{
			if (Grid[I] != ERRCellType::Void && !Seen[I])
			{
				Grid[I] = ERRCellType::Void;
			}
		}
	}

	// Safe cells: no void within VoidMargin (Chebyshev). The outline only shrinks after this point (TrimFar).
	const FRRLayoutGenWalkParams& WK = P.Walk;
	const int32 VM = WK.VoidMargin;
	for (int32 Y = 0; Y < H; ++Y)
	{
		for (int32 X = 0; X < W; ++X)
		{
			bool bSafe = true;
			for (int32 J = -VM; J <= VM && bSafe; ++J)
			{
				for (int32 I = -VM; I <= VM; ++I)
				{
					if (!IsAsteroid(X + I, Y + J))
					{
						bSafe = false;
						break;
					}
				}
			}
			Safe[Y * W + X] = bSafe ? 1 : 0;
		}
	}
	for (int32 I = 0; I < N; ++I)
	{
		if (Grid[I] != ERRCellType::Void)
		{
			++Total;
		}
	}

	// 2. Spawn clearing: wobbly disc plus the open zone square.
	const FRRLayoutGenSpawnParams& SP = P.Spawn;
	for (int32 Y = 0; Y < H; ++Y)
	{
		for (int32 X = 0; X < W; ++X)
		{
			const int32 I = Y * W + X;
			const double Limit = SP.DiscRadius + SP.DiscWobble * FMath::Sin(X * SP.DiscFreqX + Y * SP.DiscFreqY);
			if (Grid[I] == ERRCellType::Rock && Hypot(X - SX, Y - SY) <= Limit)
			{
				Grid[I] = ERRCellType::Floor;
				++FloorCount;
			}
		}
	}
	for (int32 Y = SY - SP.ZoneHalf; Y <= SY + SP.ZoneHalf; ++Y)
	{
		for (int32 X = SX - SP.ZoneHalf; X <= SX + SP.ZoneHalf; ++X)
		{
			const int32 I = Y * W + X;
			if (Grid[I] != ERRCellType::Floor)
			{
				++FloorCount;
			}
			Grid[I] = ERRCellType::Floor;
		}
	}

	// 3. Main network: wander walks from random floor cells until floor reaches FloorShare of the asteroid.
	const FRRLayoutGenMainParams& MN = P.Main;
	TArray<int32> FloorCells;
	for (int32 Walk = 0; FloorCount < MN.FloorShare * Total && Walk < MN.MaxWalks; ++Walk)
	{
		FloorCells.Reset();
		for (int32 I = 0; I < N; ++I)
		{
			if (Grid[I] == ERRCellType::Floor)
			{
				FloorCells.Add(I);
			}
		}
		const int32 Start = FloorCells[FMath::FloorToInt32(Rnd() * FloorCells.Num())];
		const double Angle = Rnd() * TwoPiApprox;
		const int32 Len = MN.StepsMin + FMath::FloorToInt32(Rnd() * (MN.StepsMax - MN.StepsMin));
		const double Rho0 = MN.Rho0Min + Rnd() * (MN.Rho0Max - MN.Rho0Min);
		ApplyCarve(WanderWalk(Start % W, Start / W, Angle, Len, Rho0, WK.Turn));
	}

	// 4. Hidden pockets: walks inside rock, kept when all rock, large enough and DistMin..DistMax from the main network.
	const FRRLayoutGenPocketParams& PK = P.Pockets;
	const TArray<int32> MainDist = Bfs(Reach());
	for (int32 Try = 0; Try < PK.Tries && PocketCount < PK.Count; ++Try)
	{
		const double X = PK.Margin + Rnd() * (W - 2 * PK.Margin);
		const double Y = PK.Margin + Rnd() * (H - 2 * PK.Margin);
		const int32 RX = RoundHalfUp(X);
		const int32 RY = RoundHalfUp(Y);
		if (!InBounds(RX, RY) || Grid[RY * W + RX] != ERRCellType::Rock)
		{
			continue;
		}
		const double Angle = Rnd() * TwoPiApprox;
		const int32 Len = PK.StepsBase + FMath::FloorToInt32(Rnd() * PK.StepsRand);
		const double Rho0 = PK.Rho0Min + Rnd() * (PK.Rho0Max - PK.Rho0Min);
		const TArray<int32> Cells = WanderWalk(X, Y, Angle, Len, Rho0, WK.Turn + PK.TurnAdd);
		if (Cells.Num() < PK.MinCells)
		{
			continue;
		}
		int32 MinDist = FarDistance;
		bool bValid = true;
		for (const int32 I : Cells)
		{
			if (Grid[I] != ERRCellType::Rock || MainDist[I] < PK.DistMin)
			{
				bValid = false;
				break;
			}
			MinDist = FMath::Min(MinDist, MainDist[I]);
		}
		if (!bValid || MinDist > PK.DistMax)
		{
			continue;
		}
		for (const int32 I : Cells)
		{
			Grid[I] = ERRCellType::Floor;
		}
		FloorCount += Cells.Num();
		MarkHidden(Cells);
		++PocketCount;
	}

	// 5. Far-cell filler.
	RunFiller();

	// 6. Cleanup: regions, filler, rock islands, then the passage repair looped with refill and filler until stable,
	// always ending on a repair (the 3-cell rule is hard; maxd is checked by the seed acceptance).
	RefillRegions();
	RunFiller();
	FillRockIslands();
	for (int32 Loop = 0; Loop < P.Cleanup.Loops; ++Loop)
	{
		const int32 Repaired = PassageRepair();
		RefillRegions();
		if (RunFiller() == 0 && Repaired == 0)
		{
			break;
		}
	}

	// 7. Review repairs (layout-review-rules): dead and too-open 24 x 24 windows, straight lanes. Each pass that changed
	// the grid is followed by the passage repair and region refill (repairs add no far cell: the filler is not rerun).
	// A layout with nothing to repair is unchanged.
	for (int32 Pass = 0; Pass < P.Review.Passes; ++Pass)
	{
		if (RepairReviewRules() == 0)
		{
			break;
		}
		PassageRepair();
		RefillRegions();
	}
	PassageRepair();
	RefillRegions();
	TrimFar();
	Grid[SY * W + SX] = ERRCellType::Spawn;
}

bool FRRLayoutGenerator::CountWindow(int32 X, int32 Y, int32& OutOpen) const
{
	const int32 K = P.Review.Window;
	OutOpen = 0;
	for (int32 J = 0; J < K; ++J)
	{
		for (int32 I = 0; I < K; ++I)
		{
			const ERRCellType Type = TypeAt(X + I, Y + J);
			if (Type == ERRCellType::Void)
			{
				return false;
			}
			OutOpen += IsOpen(Type) ? 1 : 0;
		}
	}
	return true;
}

void FRRLayoutGenerator::ScanWindows(double Low, double High, double& OutMin, double& OutMax, TArray<FReviewWindow>* OutOffending) const
{
	const FRRLayoutGenReviewParams& RV = P.Review;
	const int32 K = RV.Window;
	const int32 S = FMath::Max(1, RV.Stride);
	const int32 W1 = W + 1;
	TArray<int32> OpenSat;
	TArray<int32> VoidSat;
	OpenSat.SetNumZeroed(W1 * (H + 1));
	VoidSat.SetNumZeroed(W1 * (H + 1));
	for (int32 Y = 0; Y < H; ++Y)
	{
		for (int32 X = 0; X < W; ++X)
		{
			const ERRCellType Type = Grid[Y * W + X];
			const int32 Here = (Y + 1) * W1 + X + 1;
			OpenSat[Here] = OpenSat[Y * W1 + X + 1] + OpenSat[(Y + 1) * W1 + X] - OpenSat[Y * W1 + X] + (IsOpen(Type) ? 1 : 0);
			VoidSat[Here] = VoidSat[Y * W1 + X + 1] + VoidSat[(Y + 1) * W1 + X] - VoidSat[Y * W1 + X] + (Type == ERRCellType::Void ? 1 : 0);
		}
	}
	OutMin = 1.0;
	OutMax = 0.0;
	if (OutOffending)
	{
		OutOffending->Reset();
	}
	const double Area = static_cast<double>(K * K);
	for (int32 Y = 0; Y + K <= H; Y += S)
	{
		for (int32 X = 0; X + K <= W; X += S)
		{
			auto Box = [&](const TArray<int32>& Sat) { return Sat[(Y + K) * W1 + X + K] - Sat[Y * W1 + X + K] - Sat[(Y + K) * W1 + X] + Sat[Y * W1 + X]; };
			if (Box(VoidSat) > 0)
			{
				continue;
			}
			const int32 Open = Box(OpenSat);
			const double Share = Open / Area;
			OutMin = FMath::Min(OutMin, Share);
			OutMax = FMath::Max(OutMax, Share);
			if (OutOffending && (Share < Low || Share > High))
			{
				OutOffending->Add(FReviewWindow{ X, Y, Open, Share });
			}
		}
	}
	if (OutOffending)
	{
		OutOffending->StableSort([Low, High](const FReviewWindow& A, const FReviewWindow& B)
		{
			const double EA = A.Share < Low ? Low - A.Share : A.Share - High;
			const double EB = B.Share < Low ? Low - B.Share : B.Share - High;
			return EA > EB;
		});
	}
}

void FRRLayoutGenerator::MeasureWindows(double& OutMin, double& OutMax) const
{
	ScanWindows(0.0, 1.0, OutMin, OutMax, nullptr);
}

void FRRLayoutGenerator::BuildSpans(bool bAlongX, TArray<int32>& OutLo, TArray<int32>& OutHi) const
{
	const int32 A = bAlongX ? W : H;
	const int32 B = bAlongX ? H : W;
	OutLo.Init(-1, N);
	OutHi.Init(-1, N);
	for (int32 AI = 0; AI < A; ++AI)
	{
		int32 BI = 0;
		while (BI < B)
		{
			if (!IsOpen(Grid[LaneIndex(bAlongX, AI, BI)]))
			{
				++BI;
				continue;
			}
			int32 End = BI;
			while (End + 1 < B && IsOpen(Grid[LaneIndex(bAlongX, AI, End + 1)]))
			{
				++End;
			}
			for (int32 K = BI; K <= End; ++K)
			{
				OutLo[LaneIndex(bAlongX, AI, K)] = BI;
				OutHi[LaneIndex(bAlongX, AI, K)] = End;
			}
			BI = End + 1;
		}
	}
}

int32 FRRLayoutGenerator::ScanLanes(bool bAlongX, int32 MinLen, TArray<FLaneRun>* OutRuns) const
{
	// Scan of layout-review-rules (task-492-03 review): along the axis, the open span through the anchor cell keeps a
	// width of LaneWidthMin..LaneWidthMax and both walls within LaneTolerance cells of where the run started.
	const FRRLayoutGenReviewParams& RV = P.Review;
	TArray<int32> Lo;
	TArray<int32> Hi;
	BuildSpans(bAlongX, Lo, Hi);
	const int32 A = bAlongX ? W : H;
	const int32 B = bAlongX ? H : W;
	int32 Longest = 0;
	for (int32 BI = 0; BI < B; ++BI)
	{
		int32 Len = 0;
		int32 T0 = 0;
		int32 B0 = 0;
		int32 Start = 0;
		for (int32 AI = 0; AI <= A; ++AI)
		{
			bool bOk = false;
			int32 Top = 0;
			int32 Bot = 0;
			if (AI < A)
			{
				const int32 I = LaneIndex(bAlongX, AI, BI);
				if (IsOpen(Grid[I]))
				{
					Top = Lo[I];
					Bot = Hi[I];
					const int32 Width = Bot - Top + 1;
					bOk = Width >= RV.LaneWidthMin && Width <= RV.LaneWidthMax;
				}
			}
			if (bOk && Len > 0 && FMath::Abs(Top - T0) <= RV.LaneTolerance && FMath::Abs(Bot - B0) <= RV.LaneTolerance)
			{
				++Len;
				continue;
			}
			Longest = FMath::Max(Longest, Len);
			if (OutRuns && Len > MinLen)
			{
				OutRuns->Add(FLaneRun{ bAlongX, BI, Start, Len });
			}
			Len = bOk ? 1 : 0;
			T0 = Top;
			B0 = Bot;
			Start = AI;
		}
	}
	return Longest;
}

int32 FRRLayoutGenerator::LaneRunAt(bool bAlongX, int32 B, int32 A) const
{
	TArray<FLaneRun> Runs;
	ScanLanes(bAlongX, 0, &Runs);
	for (const FLaneRun& Run : Runs)
	{
		if (Run.B == B && A >= Run.Start && A < Run.Start + Run.Len)
		{
			return Run.Len;
		}
	}
	return 0;
}

int32 FRRLayoutGenerator::MeasureLongestLane() const
{
	return FMath::Max(ScanLanes(true, MAX_int32, nullptr), ScanLanes(false, MAX_int32, nullptr));
}

bool FRRLayoutGenerator::IsCovered(int32 X, int32 Y) const
{
	const int32 K = P.Cleanup.Passage;
	for (int32 OY = Y - K + 1; OY <= Y; ++OY)
	{
		for (int32 OX = X - K + 1; OX <= X; ++OX)
		{
			bool bAllOpen = true;
			for (int32 DY = 0; DY < K && bAllOpen; ++DY)
			{
				for (int32 DX = 0; DX < K; ++DX)
				{
					if (!IsOpen(TypeAt(OX + DX, OY + DY)))
					{
						bAllOpen = false;
						break;
					}
				}
			}
			if (bAllOpen)
			{
				return true;
			}
		}
	}
	return false;
}

bool FRRLayoutGenerator::BoxCovered(int32 X0, int32 Y0, int32 X1, int32 Y1) const
{
	for (int32 Y = FMath::Max(0, Y0); Y <= FMath::Min(H - 1, Y1); ++Y)
	{
		for (int32 X = FMath::Max(0, X0); X <= FMath::Min(W - 1, X1); ++X)
		{
			if (IsOpen(Grid[Y * W + X]) && !IsCovered(X, Y))
			{
				return false;
			}
		}
	}
	return true;
}

void FRRLayoutGenerator::Undo(const TArray<FCellUndo>& Changes)
{
	for (int32 K = Changes.Num() - 1; K >= 0; --K)
	{
		const int32 I = Changes[K].Key;
		const ERRCellType Old = Changes[K].Value;
		if (Grid[I] == ERRCellType::Floor && Old != ERRCellType::Floor)
		{
			--FloorCount;
		}
		else if (Grid[I] != ERRCellType::Floor && Old == ERRCellType::Floor)
		{
			++FloorCount;
		}
		Grid[I] = Old;
	}
}

void FRRLayoutGenerator::CarveDisc(double CX, double CY, double Rho, TArray<FCellUndo>& Changes)
{
	const double Amp = P.Review.ShapeNoise;
	const int32 R = FMath::CeilToInt32(Rho + Amp) + 1;
	const int32 RX = RoundHalfUp(CX);
	const int32 RY = RoundHalfUp(CY);
	for (int32 J = -R; J <= R; ++J)
	{
		for (int32 I = -R; I <= R; ++I)
		{
			const int32 X = RX + I;
			const int32 Y = RY + J;
			if (!IsAsteroid(X, Y))
			{
				continue;
			}
			const int32 Id = Y * W + X;
			if (Grid[Id] != ERRCellType::Rock || !Safe[Id] || HiddenGuard[Id])
			{
				continue;
			}
			if (Hypot(X - CX, Y - CY) <= Rho + Amp * (2.0 * ValueNoise(X, Y, P.Walk.BrushNoiseScale, 3) - 1.0))
			{
				Changes.Add(FCellUndo(Id, ERRCellType::Rock));
				Grid[Id] = ERRCellType::Floor;
				++FloorCount;
			}
		}
	}
}

void FRRLayoutGenerator::SettleCarve(TArray<FCellUndo>& Changes)
{
	// Carving only adds open cells, so the cells already open keep their passage window; only carved cells can be
	// unreachable (a new isolated region) or narrow. They go back to rock until every carved cell left is both.
	for (int32 Round = 0; Round < P.Cleanup.RepairRounds; ++Round)
	{
		const TArray<uint8> ReachMask = Reach();
		bool bChanged = false;
		for (const FCellUndo& Change : Changes)
		{
			const int32 I = Change.Key;
			if (Change.Value == ERRCellType::Rock && Grid[I] == ERRCellType::Floor && (!ReachMask[I] || !IsCovered(I % W, I / W)))
			{
				Grid[I] = ERRCellType::Rock;
				--FloorCount;
				bChanged = true;
			}
		}
		if (!bChanged)
		{
			break;
		}
	}
}

void FRRLayoutGenerator::StampRock(double CX, double CY, double Rho, const TArray<uint8>& ReachMask, TArray<FCellUndo>& Changes)
{
	const double Amp = P.Review.ShapeNoise;
	const int32 R = FMath::CeilToInt32(Rho + Amp) + 1;
	const int32 RX = RoundHalfUp(CX);
	const int32 RY = RoundHalfUp(CY);
	for (int32 J = -R; J <= R; ++J)
	{
		for (int32 I = -R; I <= R; ++I)
		{
			const int32 X = RX + I;
			const int32 Y = RY + J;
			if (!InBounds(X, Y))
			{
				continue;
			}
			const int32 Id = Y * W + X;
			if (Grid[Id] != ERRCellType::Floor || !ReachMask[Id] || Hidden[Id] || InSpawnZone(X, Y, P.Spawn.ZoneHalf))
			{
				continue;
			}
			if (Hypot(X - CX, Y - CY) <= Rho + Amp * (2.0 * ValueNoise(X, Y, P.Walk.BrushNoiseScale, 4) - 1.0))
			{
				Changes.Add(FCellUndo(Id, ERRCellType::Floor));
				Grid[Id] = ERRCellType::Rock;
				--FloorCount;
			}
		}
	}
}

bool FRRLayoutGenerator::RockKeepsRules(const TArray<FCellUndo>& Changes, int32 ReachBefore, int32 FarBefore) const
{
	if (Changes.Num() == 0)
	{
		return false;
	}
	int32 ReachAfter = 0;
	Reach(&ReachAfter);
	if (ReachAfter != ReachBefore - Changes.Num())
	{
		return false; // the rock cut a part of the network off
	}
	int32 X0 = W;
	int32 Y0 = H;
	int32 X1 = 0;
	int32 Y1 = 0;
	for (const FCellUndo& Change : Changes)
	{
		X0 = FMath::Min(X0, Change.Key % W);
		X1 = FMath::Max(X1, Change.Key % W);
		Y0 = FMath::Min(Y0, Change.Key / W);
		Y1 = FMath::Max(Y1, Change.Key / W);
	}
	const int32 K = P.Cleanup.Passage;
	return BoxCovered(X0 - K, Y0 - K, X1 + K, Y1 + K) && MeasureMaxFar() <= FMath::Max(FarBefore, P.Search.MaxFar);
}

bool FRRLayoutGenerator::RepairDeadWindow(const FReviewWindow& Window)
{
	// A filler walk (step 5 of the recipe) from the nearest main-network cell into the deepest rock of the window.
	const int32 K = P.Review.Window;
	const TArray<int32> Dist = BfsFromWalkable();
	int32 Target = -1;
	int32 Deepest = -1;
	for (int32 Y = Window.Y; Y < Window.Y + K; ++Y)
	{
		for (int32 X = Window.X; X < Window.X + K; ++X)
		{
			const int32 I = Y * W + X;
			if (Grid[I] == ERRCellType::Rock && Safe[I] && !HiddenGuard[I] && Dist[I] > Deepest)
			{
				Deepest = Dist[I];
				Target = I;
			}
		}
	}
	if (Target < 0)
	{
		return false;
	}
	const int32 TX = Target % W;
	const int32 TY = Target / W;
	const TArray<uint8> ReachMask = Reach();
	int32 Best = -1;
	double BestDist = 1e9;
	for (int32 I = 0; I < N; ++I)
	{
		if (ReachMask[I])
		{
			const double D = Hypot(I % W - TX, I / W - TY);
			if (D < BestDist)
			{
				BestDist = D;
				Best = I;
			}
		}
	}
	if (Best < 0)
	{
		return false;
	}
	const FRRLayoutGenFillerParams& FL = P.Filler;
	const int32 BX = Best % W;
	const int32 BY = Best / W;
	const double Angle = FMath::Atan2(static_cast<double>(TY - BY), static_cast<double>(TX - BX)) + Gauss() * FL.AimNoise;
	const int32 Len = FMath::CeilToInt32(BestDist) + FL.ExtraMin + FMath::FloorToInt32(Rnd() * FL.ExtraRand);
	const double Rho0 = FL.Rho0Min + Rnd() * (FL.Rho0Max - FL.Rho0Min);
	TArray<FCellUndo> Changes;
	for (const int32 I : WanderWalk(BX, BY, Angle, Len, Rho0, P.Walk.Turn * FL.TurnScale))
	{
		if (Grid[I] == ERRCellType::Rock)
		{
			Changes.Add(FCellUndo(I, ERRCellType::Rock));
			Grid[I] = ERRCellType::Floor;
			++FloorCount;
		}
	}
	SettleCarve(Changes);
	int32 Open = 0;
	if (!CountWindow(Window.X, Window.Y, Open) || Open <= Window.Open)
	{
		Undo(Changes);
		return false;
	}
	++RepairCarves;
	return true;
}

bool FRRLayoutGenerator::RepairOpenWindow(const FReviewWindow& Window)
{
	// Organic rock outcrop at the window cell farthest (Chebyshev) from any closed cell, clear of the walls by Passage.
	const FRRLayoutGenReviewParams& RV = P.Review;
	const int32 K = RV.Window;
	const int32 Pass = P.Cleanup.Passage;
	TArray<int32> Clear;
	Clear.SetNumUninitialized(N);
	for (int32 I = 0; I < N; ++I)
	{
		Clear[I] = IsOpen(Grid[I]) ? FarDistance : 0;
	}
	auto ClearAt = [&](int32 X, int32 Y) { return InBounds(X, Y) ? Clear[Y * W + X] : 0; };
	for (int32 Y = 0; Y < H; ++Y)
	{
		for (int32 X = 0; X < W; ++X)
		{
			int32& C = Clear[Y * W + X];
			if (C > 0)
			{
				C = FMath::Min(C, FMath::Min(FMath::Min(ClearAt(X - 1, Y), ClearAt(X - 1, Y - 1)), FMath::Min(ClearAt(X, Y - 1), ClearAt(X + 1, Y - 1))) + 1);
			}
		}
	}
	for (int32 Y = H - 1; Y >= 0; --Y)
	{
		for (int32 X = W - 1; X >= 0; --X)
		{
			int32& C = Clear[Y * W + X];
			if (C > 0)
			{
				C = FMath::Min(C, FMath::Min(FMath::Min(ClearAt(X + 1, Y), ClearAt(X + 1, Y + 1)), FMath::Min(ClearAt(X, Y + 1), ClearAt(X - 1, Y + 1))) + 1);
			}
		}
	}
	// Free outcrop: centre drawn among the window cells where a disc of OutcropRhoMin keeps Passage open cells to the walls
	// (a disc reaches Rho + ShapeNoise cells; Clear - 1 open cells surround a cell), so outcrops do not line up on the
	// most open cells; one to OutcropDiscs overlapping discs OutcropStep x Rho apart in a random direction.
	TArray<int32> Centers;
	for (int32 Y = Window.Y; Y < Window.Y + K; ++Y)
	{
		for (int32 X = Window.X; X < Window.X + K; ++X)
		{
			const int32 I = Y * W + X;
			if (Grid[I] == ERRCellType::Floor && !Hidden[I] && !InSpawnZone(X, Y, P.Spawn.ZoneHalf)
				&& (Clear[I] - 1) - Pass - RV.ShapeNoise >= RV.OutcropRhoMin)
			{
				Centers.Add(I);
			}
		}
	}
	int32 ReachBefore = 0;
	const TArray<uint8> ReachMask = Reach(&ReachBefore);
	const int32 FarBefore = MeasureMaxFar();
	if (Centers.Num() > 0)
	{
		const int32 Center = Centers[FMath::FloorToInt32(Rnd() * Centers.Num())];
		const double MaxRho = (Clear[Center] - 1) - Pass - RV.ShapeNoise;
		const double Rho = FMath::Min(RV.OutcropRhoMin + Rnd() * (RV.OutcropRhoMax - RV.OutcropRhoMin), MaxRho);
		const double Angle = Rnd() * TwoPiApprox;
		const int32 Discs = 1 + FMath::FloorToInt32(Rnd() * FMath::Max(1, RV.OutcropDiscs));
		for (int32 Count = Discs; Count >= 1; --Count)
		{
			TArray<FCellUndo> Changes;
			for (int32 D = 0; D < Count; ++D)
			{
				const double Step = D * RV.OutcropStep * Rho;
				StampRock(Center % W + FMath::Cos(Angle) * Step, Center / W + FMath::Sin(Angle) * Step, Rho, ReachMask, Changes);
			}
			int32 Open = 0;
			if (Changes.Num() > P.Cleanup.MaxRockIsland && RockKeepsRules(Changes, ReachBefore, FarBefore) && CountWindow(Window.X, Window.Y, Open)
				&& Open < Window.Open)
			{
				++RepairOutcrops;
				return true;
			}
			Undo(Changes);
		}
	}

	// No room for a free outcrop (around the spawn square for example): a bump on a wall of the window, at the wall cells
	// with the most open cells around them.
	const int32 Ring = FMath::CeilToInt32(RV.OutcropRhoMax + RV.ShapeNoise);
	TArray<TPair<int32, int32>> Walls; // (open cells around, index)
	for (int32 Y = Window.Y; Y < Window.Y + K; ++Y)
	{
		for (int32 X = Window.X; X < Window.X + K; ++X)
		{
			const int32 I = Y * W + X;
			if (Grid[I] != ERRCellType::Rock || InSpawnZone(X, Y, P.Spawn.ZoneHalf))
			{
				continue;
			}
			bool bFacesNetwork = false;
			for (const FIntPoint& D : D4)
			{
				bFacesNetwork |= InBounds(X + D.X, Y + D.Y) && ReachMask[(Y + D.Y) * W + X + D.X] != 0;
			}
			if (!bFacesNetwork)
			{
				continue;
			}
			int32 OpenAround = 0;
			for (int32 J = -Ring; J <= Ring; ++J)
			{
				for (int32 Q = -Ring; Q <= Ring; ++Q)
				{
					OpenAround += IsOpen(TypeAt(X + Q, Y + J)) ? 1 : 0;
				}
			}
			Walls.Add(TPair<int32, int32>(OpenAround, I));
		}
	}
	Walls.StableSort([](const TPair<int32, int32>& A, const TPair<int32, int32>& B) { return A.Key > B.Key; });
	for (int32 K2 = 0; K2 < FMath::Min(Walls.Num(), RV.MaxTries); ++K2)
	{
		const int32 I = Walls[K2].Value;
		const double Rho = RV.OutcropRhoMin + Rnd() * (RV.OutcropRhoMax - RV.OutcropRhoMin);
		TArray<FCellUndo> Changes;
		StampRock(I % W, I / W, Rho, ReachMask, Changes);
		int32 Open = 0;
		if (RockKeepsRules(Changes, ReachBefore, FarBefore) && CountWindow(Window.X, Window.Y, Open) && Open < Window.Open)
		{
			++RepairOutcrops;
			return true;
		}
		Undo(Changes);
	}
	return false;
}

bool FRRLayoutGenerator::RepairLane(const FLaneRun& Lane)
{
	// A wall bump (rock, lane wide enough to keep Passage open cells) or a wall niche (carved) shifts a wall by more than
	// LaneTolerance at one position, which splits the run there.
	const FRRLayoutGenReviewParams& RV = P.Review;
	const int32 Pass = P.Cleanup.Passage;
	const int32 Across = Lane.bAlongX ? H : W;
	static const int32 Num[3] = { 1, 1, 2 };
	static const int32 Den[3] = { 2, 3, 3 };
	for (int32 F = 0; F < 3; ++F)
	{
		const int32 A = Lane.Start + Lane.Len * Num[F] / Den[F];
		TArray<int32> Lo;
		TArray<int32> Hi;
		BuildSpans(Lane.bAlongX, Lo, Hi);
		const int32 Anchor = LaneIndex(Lane.bAlongX, A, Lane.B);
		if (!IsOpen(Grid[Anchor]))
		{
			continue;
		}
		const int32 Top = Lo[Anchor];
		const int32 Bot = Hi[Anchor];
		const double BumpMax = (Bot - Top + 1) - Pass - RV.ShapeNoise;
		const bool bWide = BumpMax >= RV.LaneTolerance + 1;
		for (int32 Option = 0; Option < 2; ++Option)
		{
			const bool bBump = (Option == 0) == bWide;
			for (int32 Side = 0; Side < 2; ++Side)
			{
				const int32 WallB = Side == 0 ? Top - 1 : Bot + 1;
				if (WallB < 0 || WallB >= Across)
				{
					continue;
				}
				const int32 Wall = LaneIndex(Lane.bAlongX, A, WallB);
				const int32 X = Wall % W;
				const int32 Y = Wall / W;
				double Rho = RV.LaneRhoMin + Rnd() * (RV.LaneRhoMax - RV.LaneRhoMin);
				TArray<FCellUndo> Changes;
				bool bOk = false;
				if (bBump)
				{
					Rho = FMath::Min(Rho, BumpMax);
					if (Rho < RV.LaneTolerance + 1)
					{
						continue;
					}
					int32 ReachBefore = 0;
					const TArray<uint8> ReachMask = Reach(&ReachBefore);
					const int32 FarBefore = MeasureMaxFar();
					StampRock(X, Y, Rho, ReachMask, Changes);
					bOk = RockKeepsRules(Changes, ReachBefore, FarBefore);
				}
				else
				{
					if (Grid[Wall] != ERRCellType::Rock)
					{
						continue;
					}
					CarveDisc(X, Y, Rho, Changes);
					SettleCarve(Changes);
					for (const FCellUndo& Change : Changes)
					{
						bOk |= Grid[Change.Key] == ERRCellType::Floor;
					}
				}
				if (bOk && LaneRunAt(Lane.bAlongX, Lane.B, A) < Lane.Len)
				{
					++RepairLanes;
					return true;
				}
				Undo(Changes);
			}
		}
	}
	return false;
}

int32 FRRLayoutGenerator::RepairReviewRules()
{
	const FRRLayoutGenReviewParams& RV = P.Review;
	TSet<int32> FailedWindows;
	TSet<int64> FailedLanes;
	int32 Actions = 0;
	while (Actions < RV.MaxActions)
	{
		bool bFixed = false;
		double Min = 0.0;
		double Max = 0.0;
		TArray<FReviewWindow> Windows;
		ScanWindows(RV.RepairOpenMin, RV.RepairOpenMax, Min, Max, &Windows);
		int32 Tries = 0;
		for (const FReviewWindow& Window : Windows)
		{
			const int32 Key = Window.Y * W + Window.X;
			if (FailedWindows.Contains(Key))
			{
				continue;
			}
			if (Tries++ >= RV.MaxTries)
			{
				break;
			}
			bFixed = Window.Share < RV.RepairOpenMin ? RepairDeadWindow(Window) : RepairOpenWindow(Window);
			if (bFixed)
			{
				break;
			}
			FailedWindows.Add(Key);
		}
		if (!bFixed)
		{
			TArray<FLaneRun> Lanes;
			ScanLanes(true, RV.LaneMax, &Lanes);
			ScanLanes(false, RV.LaneMax, &Lanes);
			Lanes.StableSort([](const FLaneRun& A, const FLaneRun& B) { return A.Len > B.Len; });
			Tries = 0;
			for (const FLaneRun& Lane : Lanes)
			{
				const int64 Key = (static_cast<int64>(Lane.bAlongX) << 40) | (static_cast<int64>(Lane.B) << 20) | Lane.Start;
				if (FailedLanes.Contains(Key))
				{
					continue;
				}
				if (Tries++ >= RV.MaxTries)
				{
					break;
				}
				bFixed = RepairLane(Lane);
				if (bFixed)
				{
					break;
				}
				FailedLanes.Add(Key);
			}
		}
		if (!bFixed)
		{
			break;
		}
		++Actions;
	}
	return Actions;
}

int32 FRRLayoutGenerator::MeasureMaxFar() const
{
	const TArray<int32> Dist = BfsFromWalkable();
	int32 MaxFar = 0;
	for (int32 I = 0; I < N; ++I)
	{
		if (Grid[I] != ERRCellType::Void)
		{
			MaxFar = FMath::Max(MaxFar, Dist[I]);
		}
	}
	return MaxFar;
}

bool FRRLayoutGenerator::PickSpread(const TArray<FIntPoint>& Candidates, int32 Count, int32 Tries, TFunctionRef<bool(int32, int32)> IsValid,
	TArray<FIntPoint>& Picked, TFunctionRef<void(int32, int32)> Place)
{
	for (int32 K = 0; K < Count; ++K)
	{
		int32 Best = -1;
		double BestDist = -1.0;
		for (int32 Try = 0; Try < Tries && Candidates.Num() > 0; ++Try)
		{
			const int32 C = FMath::FloorToInt32(Rnd() * Candidates.Num());
			const FIntPoint& Cell = Candidates[C];
			if (!IsValid(Cell.X, Cell.Y))
			{
				continue;
			}
			double Nearest = 1e9;
			for (const FIntPoint& Other : Picked)
			{
				Nearest = FMath::Min(Nearest, Hypot(Other.X - Cell.X, Other.Y - Cell.Y));
			}
			if (Nearest > BestDist)
			{
				BestDist = Nearest;
				Best = C;
			}
		}
		if (Best < 0)
		{
			return false;
		}
		const FIntPoint Cell = Candidates[Best];
		Place(Cell.X, Cell.Y);
		Picked.Add(Cell);
	}
	return true;
}

bool FRRLayoutGenerator::Decorate()
{
	const FRRLayoutGenHealParams& HL = P.Heal;
	const FRRLayoutGenBarrelParams& BR = P.Barrels;
	const int32 V = HL.Vein;
	const TArray<uint8> ReachMask = Reach();

	// 7. Heal veins: V x V blocks, in hidden-pocket floor first, then in walls touching the main network.
	TArray<FIntPoint> Perimeter; // 4-neighbours of a V x V block, relative to its corner
	for (int32 K = 0; K < V; ++K)
	{
		Perimeter.Add(FIntPoint(K, -1));
		Perimeter.Add(FIntPoint(K, V));
		Perimeter.Add(FIntPoint(-1, K));
		Perimeter.Add(FIntPoint(V, K));
	}
	auto BlockCells = [this, V](int32 X, int32 Y)
	{
		TArray<int32, TInlineAllocator<16>> Cells;
		for (int32 J = 0; J < V; ++J)
		{
			for (int32 I = 0; I < V; ++I)
			{
				Cells.Add((Y + J) * W + X + I);
			}
		}
		return Cells;
	};
	auto HealNear = [this, V](int32 X, int32 Y, int32 Margin)
	{
		for (int32 J = -Margin; J < V + Margin; ++J)
		{
			for (int32 I = -Margin; I < V + Margin; ++I)
			{
				if (TypeAt(X + I, Y + J) == ERRCellType::HealOre)
				{
					return true;
				}
			}
		}
		return false;
	};
	auto CoveredLocal = [this](int32 X, int32 Y)
	{
		for (int32 OY = Y - 2; OY <= Y; ++OY)
		{
			for (int32 OX = X - 2; OX <= X; ++OX)
			{
				bool bAllOpen = true;
				for (int32 DY = 0; DY < 3 && bAllOpen; ++DY)
				{
					for (int32 DX = 0; DX < 3; ++DX)
					{
						if (!IsOpen(TypeAt(OX + DX, OY + DY)))
						{
							bAllOpen = false;
							break;
						}
					}
				}
				if (bAllOpen)
				{
					return true;
				}
			}
		}
		return false;
	};
	auto ValidWall = [&](int32 X, int32 Y)
	{
		const int32 EH = HL.ExcludeHalf;
		if (InSpawnZone(X, Y, EH) || InSpawnZone(X + V - 1, Y + V - 1, EH) || InSpawnZone(X, Y + V - 1, EH) || InSpawnZone(X + V - 1, Y, EH))
		{
			return false;
		}
		for (const int32 I : BlockCells(X, Y))
		{
			if (Grid[I] != ERRCellType::Rock)
			{
				return false;
			}
		}
		if (HealNear(X, Y, 1))
		{
			return false;
		}
		int32 Rock = 0;
		bool bTouches = false;
		for (const FIntPoint& Offset : Perimeter)
		{
			const int32 PX = X + Offset.X;
			const int32 PY = Y + Offset.Y;
			const ERRCellType Type = TypeAt(PX, PY);
			if (Type == ERRCellType::Rock)
			{
				++Rock;
			}
			if (IsWalkable(Type) && ReachMask[PY * W + PX])
			{
				bTouches = true;
			}
		}
		return bTouches && Rock >= HL.WallMinRock;
	};
	auto ValidPocket = [&](int32 X, int32 Y)
	{
		const int32 EH = HL.ExcludeHalf;
		if (InSpawnZone(X, Y, EH) || InSpawnZone(X + V - 1, Y + V - 1, EH))
		{
			return false;
		}
		const auto Cells = BlockCells(X, Y);
		for (const int32 I : Cells)
		{
			if (Grid[I] != ERRCellType::Floor || ReachMask[I])
			{
				return false;
			}
		}
		if (HealNear(X, Y, 1))
		{
			return false;
		}
		int32 Rock = 0;
		for (const FIntPoint& Offset : Perimeter)
		{
			if (TypeAt(X + Offset.X, Y + Offset.Y) == ERRCellType::Rock)
			{
				++Rock;
			}
		}
		if (Rock < HL.PocketMinRock)
		{
			return false;
		}
		// Tentative placement: every open cell within 3 cells must stay in an all-open 3 x 3 window.
		for (const int32 I : Cells)
		{
			Grid[I] = ERRCellType::HealOre;
		}
		bool bOk = true;
		for (int32 J = -3; J < V + 3 && bOk; ++J)
		{
			for (int32 I = -3; I < V + 3; ++I)
			{
				if (IsOpen(TypeAt(X + I, Y + J)) && !CoveredLocal(X + I, Y + J))
				{
					bOk = false;
					break;
				}
			}
		}
		for (const int32 I : Cells)
		{
			Grid[I] = ERRCellType::Floor;
		}
		return bOk;
	};

	TArray<FIntPoint> WallCandidates;
	TArray<FIntPoint> PocketCandidates;
	for (int32 Y = 1; Y < H - V; ++Y)
	{
		for (int32 X = 1; X < W - V; ++X)
		{
			if (ValidWall(X, Y))
			{
				WallCandidates.Add(FIntPoint(X, Y));
			}
			if (ValidPocket(X, Y))
			{
				PocketCandidates.Add(FIntPoint(X, Y));
			}
		}
	}
	auto PlaceHeal = [&](int32 X, int32 Y)
	{
		for (const int32 I : BlockCells(X, Y))
		{
			Grid[I] = ERRCellType::HealOre;
		}
	};
	TArray<FIntPoint> HealPicked;
	if (!PickSpread(PocketCandidates, HL.PocketVeins, HL.Tries, ValidPocket, HealPicked, PlaceHeal))
	{
		return false;
	}
	if (!PickSpread(WallCandidates, HL.WallVeins, HL.Tries, ValidWall, HealPicked, PlaceHeal))
	{
		return false;
	}

	// Barrels: reachable floor against a rock wall (a barrel is open for the passage rule), away from the spawn and the heal
	// veins, never cutting the local floor ring; spread, PairsMin..PairsMax of them in pairs for chains.
	const TArray<uint8> ReachAfterHeal = Reach();
	static const FIntPoint Ring[8] = { FIntPoint(-1, -1), FIntPoint(0, -1), FIntPoint(1, -1), FIntPoint(1, 0),
		FIntPoint(1, 1), FIntPoint(0, 1), FIntPoint(-1, 1), FIntPoint(-1, 0) };
	auto ValidBarrel = [&](int32 X, int32 Y)
	{
		if (TypeAt(X, Y) != ERRCellType::Floor || !ReachAfterHeal[Y * W + X] || InSpawnZone(X, Y, BR.SpawnExclude))
		{
			return false;
		}
		bool bAgainstRock = false;
		for (const FIntPoint& D : D4)
		{
			bAgainstRock |= TypeAt(X + D.X, Y + D.Y) == ERRCellType::Rock;
		}
		if (!bAgainstRock)
		{
			return false;
		}
		for (int32 J = -BR.HealClear; J <= BR.HealClear; ++J)
		{
			for (int32 I = -BR.HealClear; I <= BR.HealClear; ++I)
			{
				const ERRCellType Type = TypeAt(X + I, Y + J);
				if (Type == ERRCellType::HealOre || (Type == ERRCellType::Barrel && Hypot(I, J) < BR.PairDistMin))
				{
					return false;
				}
			}
		}
		// Walkable ring cells must form one run, so no neighbour loses its link through this cell.
		int32 Segments = 0;
		for (int32 K = 0; K < 8; ++K)
		{
			const FIntPoint& Cur = Ring[K];
			const FIntPoint& Prev = Ring[(K + 7) % 8];
			if (IsWalkable(TypeAt(X + Cur.X, Y + Cur.Y)) && !IsWalkable(TypeAt(X + Prev.X, Y + Prev.Y)))
			{
				++Segments;
			}
		}
		return Segments == 1;
	};
	TArray<FIntPoint> BarrelCandidates;
	for (int32 Y = 0; Y < H; ++Y)
	{
		for (int32 X = 0; X < W; ++X)
		{
			if (ValidBarrel(X, Y))
			{
				BarrelCandidates.Add(FIntPoint(X, Y));
			}
		}
	}
	const int32 BarrelCount = BR.Min + FMath::FloorToInt32(Rnd() * (BR.Max - BR.Min + 1));
	const int32 PairCount = BR.PairsMin + FMath::FloorToInt32(Rnd() * (BR.PairsMax - BR.PairsMin + 1));
	TArray<FIntPoint> Barrels;
	auto PlaceBarrel = [this](int32 X, int32 Y) { Grid[Y * W + X] = ERRCellType::Barrel; };
	for (int32 Pair = 0; Pair < PairCount; ++Pair)
	{
		if (!PickSpread(BarrelCandidates, 1, BR.Tries, ValidBarrel, Barrels, PlaceBarrel))
		{
			return false;
		}
		const FIntPoint First = Barrels.Last();
		const int32 R = FMath::CeilToInt32(BR.PairDistMax);
		TArray<FIntPoint> Options;
		for (int32 J = -R; J <= R; ++J)
		{
			for (int32 I = -R; I <= R; ++I)
			{
				const double D = Hypot(I, J);
				if (D >= BR.PairDistMin && D <= BR.PairDistMax && ValidBarrel(First.X + I, First.Y + J))
				{
					Options.Add(FIntPoint(First.X + I, First.Y + J));
				}
			}
		}
		if (Options.Num() == 0)
		{
			return false;
		}
		const FIntPoint Second = Options[FMath::FloorToInt32(Rnd() * Options.Num())];
		PlaceBarrel(Second.X, Second.Y);
		Barrels.Add(Second);
	}
	return PickSpread(BarrelCandidates, BarrelCount - 2 * PairCount, BR.Tries, ValidBarrel, Barrels, PlaceBarrel);
}

const TCHAR* FRRLayoutGenerator::TypeName(ERRCellType Type)
{
	switch (Type)
	{
	case ERRCellType::Void: return TEXT("void");
	case ERRCellType::Rock: return TEXT("rock");
	case ERRCellType::Ore: return TEXT("ore");
	case ERRCellType::HealOre: return TEXT("heal_ore");
	case ERRCellType::SpecialOre: return TEXT("special_ore");
	case ERRCellType::Barrel: return TEXT("barrel");
	case ERRCellType::Spawn: return TEXT("spawn");
	default: return TEXT("");
	}
}

FRRLayoutGenStats FRRLayoutGenerator::Measure() const
{
	FRRLayoutGenStats Stats;
	int32 Blocks = 0;
	int32 Floor = 0;
	for (int32 I = 0; I < N; ++I)
	{
		const ERRCellType Type = Grid[I];
		if (Type == ERRCellType::Rock || Type == ERRCellType::HealOre || Type == ERRCellType::Barrel || Type == ERRCellType::Ore
			|| Type == ERRCellType::SpecialOre)
		{
			++Blocks;
		}
		else if (Type != ERRCellType::Void)
		{
			++Floor;
		}
		const TCHAR* Name = TypeName(Type);
		if (*Name)
		{
			Stats.Counts.FindOrAdd(Name) += 1;
		}
	}
	Stats.Asteroid = Total;
	Stats.Coverage = Total > 0 ? static_cast<double>(Blocks) / Total : 0.0;
	Stats.HealOre = Stats.Counts.FindRef(TEXT("heal_ore"));
	Stats.Barrels = Stats.Counts.FindRef(TEXT("barrel"));

	int32 ReachCount = 0;
	const TArray<FRegion> Regions = FindRegions(nullptr, &ReachCount);
	Stats.Reach = Floor > 0 ? static_cast<double>(ReachCount) / Floor : 0.0;
	for (const FRegion& Region : Regions)
	{
		if (Region.Cells.Num() >= P.Cleanup.MinRegion && Region.Gap >= 1 && Region.Gap <= P.Cleanup.MaxGap)
		{
			++Stats.Pockets;
		}
	}

	// Asteroid pieces.
	TArray<uint8> Seen;
	Seen.SetNumZeroed(N);
	TArray<int32> Stack;
	for (int32 I0 = 0; I0 < N; ++I0)
	{
		if (Grid[I0] == ERRCellType::Void || Seen[I0])
		{
			continue;
		}
		++Stats.Pieces;
		Seen[I0] = 1;
		Stack.Add(I0);
		while (Stack.Num() > 0)
		{
			const int32 I = Stack.Pop(EAllowShrinking::No);
			const int32 X = I % W;
			const int32 Y = I / W;
			for (const FIntPoint& D : D4)
			{
				const int32 NX = X + D.X;
				const int32 NY = Y + D.Y;
				if (IsAsteroid(NX, NY) && !Seen[NY * W + NX])
				{
					Seen[NY * W + NX] = 1;
					Stack.Add(NY * W + NX);
				}
			}
		}
	}

	for (const FRegion& Region : Regions)
	{
		if (Region.Cells.Num() >= P.Cleanup.MinRegion)
		{
			Stats.FarthestPocket = FMath::Max(Stats.FarthestPocket, Region.Gap + 1);
		}
	}
	MeasureWindows(Stats.WindowMin, Stats.WindowMax);
	Stats.LongestLane = MeasureLongestLane();
	Stats.MaxFar = MeasureMaxFar();
	Stats.Narrow = CountNarrowCells();
	for (const FOutlineRun& Run : FindOutlineRuns(0))
	{
		Stats.OutlineRun = FMath::Max(Stats.OutlineRun, Run.Cells.Num());
	}
	return Stats;
}

TArray<FString> FRRLayoutGenerator::Check(const FRRLayoutGenStats& Stats, const FRRLayoutBrief* Brief) const
{
	const FRRLayoutGenSearchParams& S = P.Search;
	TArray<FString> Fails;
	if (Stats.Pieces != S.Pieces)
	{
		Fails.Add(FString::Printf(TEXT("pieces %d"), Stats.Pieces));
	}
	if (Stats.OutlineRun > S.MaxOutlineRun)
	{
		Fails.Add(FString::Printf(TEXT("outline run %d"), Stats.OutlineRun));
	}
	if (Stats.MaxFar > S.MaxFar)
	{
		Fails.Add(FString::Printf(TEXT("far cell %d"), Stats.MaxFar));
	}
	if (Stats.Asteroid < S.AsteroidMin || Stats.Asteroid > S.AsteroidMax)
	{
		Fails.Add(FString::Printf(TEXT("asteroid %d"), Stats.Asteroid));
	}
	if (Stats.Reach < S.ReachMin || Stats.Reach > S.ReachMax)
	{
		Fails.Add(FString::Printf(TEXT("reach %.3f"), Stats.Reach));
	}
	if (Stats.Pockets < S.MinPockets)
	{
		Fails.Add(FString::Printf(TEXT("pockets %d"), Stats.Pockets));
	}
	if (Stats.Narrow > 0)
	{
		Fails.Add(FString::Printf(TEXT("narrow cells %d"), Stats.Narrow));
	}
	const FRRLayoutGenReviewParams& RV = P.Review;
	if (Stats.WindowMin < RV.OpenMin || Stats.WindowMax > RV.OpenMax)
	{
		Fails.Add(FString::Printf(TEXT("window %.3f-%.3f"), Stats.WindowMin, Stats.WindowMax));
	}
	if (Stats.LongestLane > RV.LaneMax)
	{
		Fails.Add(FString::Printf(TEXT("lane %d"), Stats.LongestLane));
	}
	if (Stats.FarthestPocket > RV.PocketMaxDist)
	{
		Fails.Add(FString::Printf(TEXT("pocket distance %d"), Stats.FarthestPocket));
	}
	if (Brief)
	{
		if (Brief->Width && Brief->Width != P.Width)
		{
			Fails.Add(TEXT("brief width"));
		}
		if (Brief->Height && Brief->Height != P.Height)
		{
			Fails.Add(TEXT("brief height"));
		}
		if (Brief->MinPassageWidth > P.Cleanup.Passage)
		{
			Fails.Add(FString::Printf(TEXT("brief min passage %d above params passage %d"), Brief->MinPassageWidth, P.Cleanup.Passage));
		}
		if (Brief->bHasRockCoverage && (Stats.Coverage < Brief->RockCoverageMin || Stats.Coverage > Brief->RockCoverageMax))
		{
			Fails.Add(FString::Printf(TEXT("coverage %.3f"), Stats.Coverage));
		}
		if (Brief->bHasReachableFloorMin && Stats.Reach < Brief->ReachableFloorMin)
		{
			Fails.Add(TEXT("brief reach"));
		}
		for (const TPair<FString, FIntPoint>& Bound : Brief->Counts)
		{
			const int32 Count = Stats.Counts.FindRef(Bound.Key);
			if (Count < Bound.Value.X || Count > Bound.Value.Y)
			{
				Fails.Add(FString::Printf(TEXT("%s %d"), *Bound.Key, Count));
			}
		}
	}
	return Fails;
}

FRRGeneratedLayout FRRLayoutGenerator::RunSeed(int32 Seed, const FRRLayoutBrief* Brief)
{
	const double Start = FPlatformTime::Seconds();
	Carve(Seed);
	const int32 MaxFar = MeasureMaxFar(); // distance to floor is measured before heal veins and barrels (recipe)
	const bool bDecorated = Decorate();
	FRRGeneratedLayout Result;
	Result.Seed = Seed;
	Result.Width = W;
	Result.Height = H;
	Result.Stats = Measure();
	Result.Stats.MaxFar = MaxFar;
	Result.Stats.RepairCarves = RepairCarves;
	Result.Stats.RepairOutcrops = RepairOutcrops;
	Result.Stats.RepairLanes = RepairLanes;
	if (bDecorated)
	{
		Result.Fails = Check(Result.Stats, Brief);
	}
	else
	{
		Result.Fails.Add(TEXT("decoration quotas"));
	}
	Result.Cells = Grid;
	Result.Seconds = FPlatformTime::Seconds() - Start;
	return Result;
}

FRRGeneratedLayout FRRLayoutGenerator::Generate(int32 FirstSeed, const FRRLayoutBrief* Brief)
{
	const double Start = FPlatformTime::Seconds();
	const int32 MaxSeeds = FMath::Max(1, P.Search.MaxSeeds);
	FRRGeneratedLayout Result;
	for (int32 K = 0; K < MaxSeeds; ++K)
	{
		Result = RunSeed(FirstSeed + K, Brief);
		const FRRLayoutGenStats& S = Result.Stats;
		const FString Verdict = Result.Fails.Num() > 0 ? TEXT("FAIL: ") + FString::Join(Result.Fails, TEXT(", ")) : FString(TEXT("OK"));
		UE_LOG(LogRogueRigs, Display, TEXT("RRLayoutGen seed %d: %.2f s, asteroid %d, coverage %.3f, reachable %.3f, pockets %d, maxd %d, outline %d, heal_ore %d, barrel %d, window %.3f-%.3f, lane %d, pocket distance %d, repairs carve %d outcrop %d lane %d -- %s"),
			Result.Seed, Result.Seconds, S.Asteroid, S.Coverage, S.Reach, S.Pockets, S.MaxFar, S.OutlineRun, S.HealOre, S.Barrels,
			S.WindowMin, S.WindowMax, S.LongestLane, S.FarthestPocket, S.RepairCarves, S.RepairOutcrops, S.RepairLanes, *Verdict);
		Result.SeedsTried = K + 1;
		if (Result.Fails.Num() == 0)
		{
			break;
		}
	}
	Result.SearchSeconds = FPlatformTime::Seconds() - Start;
	return Result;
}

FString FRRLayoutGenerator::ToJson(const FString& Name, const FRRGeneratedLayout& Layout)
{
	FString Out;
	Out.Reserve(Layout.Cells.Num() * 40);
	Out += FString::Printf(TEXT("{\"version\":1,\"name\":\"%s\",\"width\":%d,\"height\":%d,\"cells\":["), *Name, Layout.Width, Layout.Height);
	bool bFirst = true;
	for (int32 Y = 0; Y < Layout.Height; ++Y)
	{
		for (int32 X = 0; X < Layout.Width; ++X)
		{
			const TCHAR* Type = TypeName(Layout.Cells[Y * Layout.Width + X]);
			if (!*Type)
			{
				continue;
			}
			if (!bFirst)
			{
				Out += TEXT(",");
			}
			bFirst = false;
			Out += FString::Printf(TEXT("{\"x\":%d,\"y\":%d,\"type\":\"%s\"}"), X, Y, Type);
		}
	}
	Out += TEXT("]}");
	return Out;
}
