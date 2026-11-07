// Rogue Rigs - deterministic 240 x 240 layout generator (C++ port of Tools/layout_gen/arena_gen_fine.js).

#include "Grid/RRLayoutGenerator.h"

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
	Mask.SetNumZeroed(N);
	TArray<int32> Stack;
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
	return Mask;
}

TArray<int32> FRRLayoutGenerator::Bfs(const TArray<uint8>& Source) const
{
	TArray<int32> Dist;
	Dist.Init(FarDistance, N);
	TArray<int32> Queue;
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
	return Dist;
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
	const FRRLayoutGenFillerParams& FL = P.Filler;
	int32 Iteration = 0;
	for (; Iteration < FL.MaxIter; ++Iteration)
	{
		const TArray<int32> Dist = BfsFromWalkable();
		TArray<int32> Far;
		for (int32 I = 0; I < N; ++I)
		{
			if (Grid[I] != ERRCellType::Void && Dist[I] > FL.MaxD)
			{
				Far.Add(I);
			}
		}
		if (Far.Num() == 0)
		{
			break;
		}
		const int32 Target = Far[FMath::FloorToInt32(Rnd() * Far.Num())];
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
		const int32 BX = Best % W;
		const int32 BY = Best / W;
		const double Angle = FMath::Atan2(static_cast<double>(TY - BY), static_cast<double>(TX - BX)) + Gauss() * FL.AimNoise;
		const int32 Len = FMath::CeilToInt32(BestDist) + FL.ExtraMin + FMath::FloorToInt32(Rnd() * FL.ExtraRand);
		const double Rho0 = FL.Rho0Min + Rnd() * (FL.Rho0Max - FL.Rho0Min);
		ApplyCarve(WanderWalk(BX, BY, Angle, Len, Rho0, P.Walk.Turn * FL.TurnScale));
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
	uint32 Value = 2166136261u;
	for (const ERRCellType Type : Grid)
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
	PassageRepair();
	RefillRegions();
	TrimFar();
	Grid[SY * W + SX] = ERRCellType::Spawn;
}
