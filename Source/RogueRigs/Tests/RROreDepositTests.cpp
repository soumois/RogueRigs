// Rogue Rigs - automation tests of the ore deposits (task-070-01, task-088-02): ore type table, 2 x 2 Duskite placement, whole-deposit mining, drops, the pickup spawn cap and the 2 x 2 heal veins.

#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "RogueRigs.h"
#include "Drone/RRDrone.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GAS/RRAbilitySystemComponent.h"
#include "Grid/RRArenaGrid.h"
#include "Grid/RROreTypeRow.h"
#include "Objectives/RRConsole.h"
#include "Pickups/RRPickup.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestEventRecorder.h"
#include "Tests/RRTestHarness.h"
#include "Tests/RRTestStatsRows.h"
#include "UObject/StrongObjectPtr.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RROreDepositTests
{
	using namespace RRTestHarness;
	using namespace RRTestAccess;

	/** Ore id of Duskite in OreTypes.csv. */
	const FName DuskiteId(TEXT("Duskite"));

	/** Ore id of the heal veins in OreTypes.csv (task-088-02). */
	const FName HealId(TEXT("Heal"));

	/** Blueprint heal pickup, set on the grid by the heal vein test. */
	const TCHAR* const HealPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");

	/** A block of the 2 x 2 heal vein (147-148, 135-136) of arena_m1_fine (task-088-01). */
	const FIntPoint HealVeinCell(148, 136);

	/** Floor cell north of that vein, where the heal vein test stands the miner. */
	const FIntPoint HealVeinNorthFloorCell(148, 134);

	/** Blueprint ore pickup, used when BP_ArenaGrid has no OrePickupClass. */
	const TCHAR* const OrePickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupOre.BP_PickupOre_C");

	/** Spawns BP_ArenaGrid on the game layout (arena_m1_fine, 67 uu) with ore deposits on or off and the given seed. */
	ARRArenaGrid* SpawnOreGrid(FAutomationTestBase& Test, const FTestWorld& TestWorld, bool bPlaceDeposits, int32 Seed = 1)
	{
		UClass* GridClass = LoadTestClass<ARRArenaGrid>(Test, ArenaGridClassPath);
		if (!GridClass)
		{
			return nullptr;
		}
		const FTransform Origin(FVector::ZeroVector);
		ARRArenaGrid* Grid = TestWorld.Get()->SpawnActorDeferred<ARRArenaGrid>(GridClass, Origin, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Test.TestNotNull(TEXT("grid spawned"), Grid))
		{
			return nullptr;
		}
		FGridAccess::LayoutNameOf(Grid) = FineLayoutName;
		FGridAccess::CellSizeOf(Grid) = FineCellSize;
		FGridAccess::PlaceOreDepositsOf(Grid) = bPlaceDeposits;
		FGridAccess::OreSeedOf(Grid) = Seed;
		if (!FGridAccess::OreClassRef(Grid))
		{
			FGridAccess::OreClassRef(Grid) = LoadTestClass<ARRPickup>(Test, OrePickupClassPath);
		}
		Grid->FinishSpawning(Origin);
		Test.TestTrue(TEXT("fine layout loaded"), Grid->GetGridSize() == FIntPoint(240, 240));
		return Grid;
	}

	/** Ids of every deposit placed from the ore table. */
	TArray<int32> GetTableDepositIds(const ARRArenaGrid* Grid)
	{
		TArray<int32> Ids;
		for (int32 Id = 0; Id < Grid->GetDepositCount(); ++Id)
		{
			if (Grid->GetDeposit(Id)->bPlacedFromTable)
			{
				Ids.Add(Id);
			}
		}
		return Ids;
	}

	/** Live Ore pickups in the world; OutValueSum receives the sum of their ore values. */
	/**
	 * Loot over the dig (task-235-01): a dug 2 x 2 deposit pops pickup k at dig progress k / 4, so 4 pops 0.5 s (30 frames)
	 * apart, the last on BreakFrame. PopFrames = frame at which each pickup popped.
	 */
	void CheckPopTiming(FAutomationTestBase& Test, const TArray<int32>& PopFrames, int32 BreakFrame)
	{
		FString Frames;
		for (const int32 Frame : PopFrames)
		{
			Frames += FString::Printf(TEXT(" %d"), Frame);
		}
		Test.AddInfo(FString::Printf(TEXT("pickups popped at frames%s; break frame %d"), *Frames, BreakFrame));
		if (!Test.TestEqual(TEXT("4 pickups popped during the dig"), PopFrames.Num(), 4))
		{
			return;
		}
		Test.TestEqual(TEXT("the last pickup pops on the break frame"), PopFrames.Last(), BreakFrame);
		for (int32 Pop = 1; Pop < PopFrames.Num(); ++Pop)
		{
			const int32 Gap = PopFrames[Pop] - PopFrames[Pop - 1];
			Test.TestTrue(FString::Printf(TEXT("pickups %d and %d are 0.5 s apart (%d frames, 30 +- 1)"), Pop, Pop + 1, Gap), Gap >= 29 && Gap <= 31);
		}
	}

	int32 CountOrePickups(UWorld* World, int32& OutValueSum)
	{
		int32 Count = 0;
		OutValueSum = 0;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			// task-092-01: idle pool pickups are not live pickups.
			if (URRActorPoolSubsystem::IsPooledIdle(*It))
			{
				continue;
			}
			if (IsValid(*It) && It->GetPickupType() == ERRPickupType::Ore)
			{
				++Count;
				OutValueSum += It->GetOreValue();
			}
		}
		return Count;
	}

	/** Mean world center of a deposit's cells. */
	FVector GetDepositCenter(const ARRArenaGrid* Grid, const FRRGridDeposit& Deposit)
	{
		FVector Sum = FVector::ZeroVector;
		for (const FIntPoint& Cell : Deposit.Cells)
		{
			Sum += Grid->GetCellCenter(Cell.X, Cell.Y);
		}
		return Deposit.Cells.Num() > 0 ? Sum / Deposit.Cells.Num() : Sum;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRROreTableRowTest, "RogueRigs.Ore.TableRow",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRROreTableRowTest::RunTest(const FString& Parameters)
{
	using namespace RROreDepositTests;

	// OreTypes.csv, row Duskite (owner 2026-10-10, renamed 2026-10-11): a share of the rock, square deposits, a chunk value, no colour (alpha 0, violet material kept).
	// The numbers live in the CSV (task-420): the row must be usable; RogueRigs.Ore.Placement derives its checks from it.
	FRROreTypeRow Duskite;
	if (!TestTrue(TEXT("row Duskite found in OreTypes.csv"), ARRArenaGrid::GetOreType(DuskiteId, Duskite)))
	{
		return false;
	}
	TestEqual(TEXT("Duskite is the Ore block type"), Duskite.CellType, ERRCellType::Ore);
	TestTrue(TEXT("Duskite share of rock in (0, 1)"), Duskite.RockShare > 0.f && Duskite.RockShare < 1.f);
	TestTrue(TEXT("Duskite deposits are at least 1 x 1"), Duskite.GroupSize >= 1);
	TestTrue(TEXT("Duskite chunk value > 0"), Duskite.ChunkValue > 0);
	TestTrue(TEXT("Duskite writes no colour (alpha 0)"), Duskite.Color.A <= 0.f);
	AddExpectedError(TEXT("row 'Silver' not found"), EAutomationExpectedErrorFlags::Contains, 1);
	FRROreTypeRow Missing;
	TestFalse(TEXT("an unknown ore id is not found"), ARRArenaGrid::GetOreType(TEXT("Silver"), Missing));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRROrePlacementTest, "RogueRigs.Ore.Placement",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRROrePlacementTest::RunTest(const FString& Parameters)
{
	using namespace RROreDepositTests;

	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnOreGrid(*this, TestWorld, /*bPlaceDeposits*/ true);
	ARRArenaGrid* Plain = SpawnOreGrid(*this, TestWorld, /*bPlaceDeposits*/ false);
	ARRArenaGrid* Same = SpawnOreGrid(*this, TestWorld, /*bPlaceDeposits*/ true);
	if (!Grid || !Plain || !Same)
	{
		return false;
	}

	// Count: RockShare of the layout rock in GroupSize x GroupSize groups (Duskite row of OreTypes.csv).
	const FRROreTypeRow DuskiteRow = RRTestStats::OreRow(*this, RRTestStats::DuskiteRow);
	const int32 GroupCells = DuskiteRow.GroupSize * DuskiteRow.GroupSize;
	if (!TestTrue(TEXT("CSV: Duskite GroupSize >= 1"), GroupCells >= 1))
	{
		return false;
	}
	int32 PlainRock = 0;
	for (int32 Y = 0; Y < 240; ++Y)
	{
		for (int32 X = 0; X < 240; ++X)
		{
			PlainRock += Plain->GetCellType(FIntPoint(X, Y)) == ERRCellType::Rock ? 1 : 0;
		}
	}
	const int32 Expected = FMath::RoundToInt32(DuskiteRow.RockShare * PlainRock / static_cast<float>(GroupCells));
	const TArray<int32> Ids = GetTableDepositIds(Grid);
	const int32 Placed = Grid->GetPlacedDepositCount(DuskiteId);
	UE_LOG(LogRogueRigs, Display, TEXT("RogueRigs.Ore.Placement: %d Duskite deposits on arena_m1_fine (%d rock blocks, expected %d)."), Placed, PlainRock, Expected);
	AddInfo(FString::Printf(TEXT("%d Duskite deposits on arena_m1_fine (%d rock blocks, expected %d)"), Placed, PlainRock, Expected));
	TestEqual(TEXT("placed count = RockShare x rock / GroupSize^2"), Placed, Expected);
	TestEqual(TEXT("one table deposit per placed count"), Ids.Num(), Placed);
	TestEqual(TEXT("no deposit without the table"), GetTableDepositIds(Plain).Num(), 0);

	// Shape and overlap: each deposit is a 2 x 2 square of Duskite, every cell was rock in the plain layout (never heal ore,
	// barrel, floor or void), no cell in two deposits, and the cell's id is its deposit.
	TSet<FIntPoint> Seen;
	int32 Errors = 0;
	for (const int32 Id : Ids)
	{
		const FRRGridDeposit& Deposit = *Grid->GetDeposit(Id);
		FIntPoint Min(TNumericLimits<int32>::Max(), TNumericLimits<int32>::Max());
		FIntPoint Max(TNumericLimits<int32>::Min(), TNumericLimits<int32>::Min());
		bool bOk = Deposit.Cells.Num() == GroupCells && Deposit.OreId == DuskiteId && Deposit.CellType == ERRCellType::Ore && Deposit.ChunkValue == DuskiteRow.ChunkValue;
		for (const FIntPoint& Cell : Deposit.Cells)
		{
			Min = FIntPoint(FMath::Min(Min.X, Cell.X), FMath::Min(Min.Y, Cell.Y));
			Max = FIntPoint(FMath::Max(Max.X, Cell.X), FMath::Max(Max.Y, Cell.Y));
			bool bAlreadySeen = false;
			Seen.Add(Cell, &bAlreadySeen);
			bOk &= !bAlreadySeen && Grid->GetCellType(Cell) == ERRCellType::Ore && Plain->GetCellType(Cell) == ERRCellType::Rock
				&& Grid->GetDepositId(Cell) == Id && Grid->IsTableDepositCell(Cell);
		}
		bOk &= Max - Min == FIntPoint(DuskiteRow.GroupSize - 1, DuskiteRow.GroupSize - 1);
		if (!bOk && ++Errors <= 5)
		{
			AddError(FString::Printf(TEXT("deposit %d is not a GroupSize square of Duskite on rock (%d cells, from %s to %s)"), Id, Deposit.Cells.Num(), *Min.ToString(), *Max.ToString()));
		}
	}
	TestEqual(TEXT("every deposit is a GroupSize square of Duskite on former rock"), Errors, 0);
	TestEqual(TEXT("Duskite cells = GroupSize^2 x deposits"), Seen.Num(), GroupCells * Ids.Num());

	// Heal veins and barrels are untouched: same cells as the plain build.
	int32 Changed = 0;
	for (int32 Y = 0; Y < 240; ++Y)
	{
		for (int32 X = 0; X < 240; ++X)
		{
			const ERRCellType PlainType = Plain->GetCellType(FIntPoint(X, Y));
			if ((PlainType == ERRCellType::HealOre || PlainType == ERRCellType::Barrel) && Grid->GetCellType(FIntPoint(X, Y)) != PlainType)
			{
				++Changed;
			}
		}
	}
	TestEqual(TEXT("heal ore and barrel cells unchanged"), Changed, 0);

	// Deterministic: same layout, table and seed give the same deposits.
	const TArray<int32> SameIds = GetTableDepositIds(Same);
	bool bSame = SameIds.Num() == Ids.Num();
	for (int32 Index = 0; bSame && Index < Ids.Num(); ++Index)
	{
		bSame = Grid->GetDeposit(Ids[Index])->Cells == Same->GetDeposit(SameIds[Index])->Cells;
	}
	TestTrue(TEXT("same seed, same deposits"), bSame);

	// Mining unit: every block of a deposit returns its 4 blocks, never a touching deposit's (deposits may touch).
	int32 Touching = 0;
	int32 WrongUnits = 0;
	TArray<FIntPoint> Unit;
	for (const int32 Id : Ids)
	{
		for (const FIntPoint& Cell : Grid->GetDeposit(Id)->Cells)
		{
			if (Grid->GetDepositCells(Cell, Unit) != 4 || !Unit.ContainsByPredicate([Id, Grid](const FIntPoint& C) { return Grid->GetDepositId(C) == Id; }))
			{
				++WrongUnits;
			}
			for (const FIntPoint Step : { FIntPoint(1, 0), FIntPoint(0, 1) })
			{
				const int32 Other = Grid->GetDepositId(Cell + Step);
				Touching += Other != INDEX_NONE && Other != Id ? 1 : 0;
			}
		}
	}
	AddInfo(FString::Printf(TEXT("%d touching deposit edges"), Touching));
	TestEqual(TEXT("each deposit block mines exactly its 4 blocks"), WrongUnits, 0);

	// Consoles mark every rock in their zone, table deposit blocks included (objective-console, owner 2026-10-13).
	if (Ids.Num() > 0)
	{
		const FIntPoint Center = Grid->GetDeposit(Ids[0])->Cells[0];
		const TArray<FIntPoint> Markable = GetDefault<ARRConsole>()->FindMarkableCells(Grid, Center);
		TestTrue(TEXT("console marks rocks around a deposit"), Markable.Num() > 0);
		TestTrue(TEXT("console marks the deposit block at its centre"), Markable.Contains(Center));
	}

	// Colour (task-100-01): row Duskite has alpha 0, so no deposit carries a colour and the grid writes no ore colour into the
	// instance custom data; the crystals keep the violet of their own material.
	int32 Coloured = 0;
	for (const int32 Id : Ids)
	{
		Coloured += Grid->GetDeposit(Id)->bHasColor ? 1 : 0;
	}
	TestEqual(TEXT("no Duskite deposit writes a colour"), Coloured, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRROreMineDepositTest, "RogueRigs.Ore.MineDeposit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRROreMineDepositTest::RunTest(const FString& Parameters)
{
	using namespace RROreDepositTests;

	// The miner stands on a floor cell touching a Duskite deposit block from the north, with no input: the drone digs the arc,
	// the whole deposit takes the damage, shakes together and breaks at once after 2 s; its 4 chunks of value 1 pop one by one
	// during the dig, every 0.5 s, the last on the break frame (task-235-01).
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnOreGrid(*this, TestWorld, /*bPlaceDeposits*/ true);
	if (!Grid)
	{
		return false;
	}
	int32 TargetId = INDEX_NONE;
	FIntPoint StandCell(-1, -1);
	for (const int32 Id : GetTableDepositIds(Grid))
	{
		for (const FIntPoint& Cell : Grid->GetDeposit(Id)->Cells)
		{
			const FIntPoint North = Cell - FIntPoint(0, 1);
			// Open 3 x 2 area north of the block so the capsule stands on floor.
			bool bOpen = true;
			for (int32 DX = -1; DX <= 1 && bOpen; ++DX)
			{
				for (int32 DY = 0; DY <= 1 && bOpen; ++DY)
				{
					const ERRCellType Type = Grid->GetCellType(North + FIntPoint(DX, -DY));
					bOpen = Type == ERRCellType::Floor || Type == ERRCellType::Spawn;
				}
			}
			if (bOpen)
			{
				TargetId = Id;
				StandCell = North;
				break;
			}
		}
		if (TargetId != INDEX_NONE)
		{
			break;
		}
	}
	if (!TestTrue(TEXT("a Duskite deposit with open floor to its north exists"), TargetId != INDEX_NONE))
	{
		return false;
	}
	const TArray<FIntPoint> DepositCells = Grid->GetDeposit(TargetId)->Cells;
	AddInfo(FString::Printf(TEXT("deposit %d at %s, miner on %s"), TargetId, *DepositCells[0].ToString(), *StandCell.ToString()));

	ARRMiner* Miner = SpawnMiner(*this, TestWorld, Grid, StandCell);
	ARRDrone* Drone = Miner ? Miner->GetDrone() : nullptr;
	if (!TestNotNull(TEXT("BP_Miner spawned its drone"), Drone))
	{
		return false;
	}
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}

	// Every deposit dug by the arc, recorded each frame; frames at which each target cell broke; frames at which the target
	// deposit popped each of its pickups (task-235-01: pickup k at dig progress k / 4).
	TArray<int32> PopFrames;
	TSet<int32> DugDeposits;
	TArray<int32> BreakFrames;
	BreakFrames.Init(INDEX_NONE, DepositCells.Num());
	bool bAllShookTogether = false;
	for (int32 Frame = 1; Frame <= 180; ++Frame)
	{
		TestWorld.Tick();
		while (PopFrames.Num() < Grid->GetDeposit(TargetId)->PoppedPickups)
		{
			PopFrames.Add(Frame);
		}
		for (const FIntPoint& Cell : Drone->GetDigCells())
		{
			const int32 Id = Grid->GetDepositId(Cell);
			if (Id != INDEX_NONE)
			{
				DugDeposits.Add(Id);
			}
		}
		if (Frame == 30)
		{
			bAllShookTogether = true;
			for (const FIntPoint& Cell : DepositCells)
			{
				bAllShookTogether &= FGridAccess::Shaking(Grid).Contains(Cell);
			}
		}
		bool bAllBroken = true;
		for (int32 Index = 0; Index < DepositCells.Num(); ++Index)
		{
			if (BreakFrames[Index] == INDEX_NONE && !Grid->IsBlockCell(DepositCells[Index]))
			{
				BreakFrames[Index] = Frame;
			}
			bAllBroken &= BreakFrames[Index] != INDEX_NONE;
		}
		if (bAllBroken)
		{
			break;
		}
	}
	TestTrue(TEXT("the deposit was dug"), DugDeposits.Contains(TargetId));
	TestTrue(TEXT("the 4 blocks shake together while mined"), bAllShookTogether);
	AddInfo(FString::Printf(TEXT("break frames %d %d %d %d"), BreakFrames[0], BreakFrames[1], BreakFrames[2], BreakFrames[3]));
	for (int32 Index = 0; Index < DepositCells.Num(); ++Index)
	{
		TestEqual(FString::Printf(TEXT("block %s breaks with the first"), *DepositCells[Index].ToString()), BreakFrames[Index], BreakFrames[0]);
	}
	TestTrue(TEXT("the deposit breaks after about 2 s"), BreakFrames[0] >= 119 && BreakFrames[0] <= 121);
	CheckPopTiming(*this, PopFrames, BreakFrames[0]);

	// Drops: 4 chunks of value 1 per broken deposit; another deposit of the arc still standing has popped its share so far.
	for (int32 Frame = 0; Frame < 4; ++Frame)
	{
		TestWorld.Tick();
	}
	int32 Broken = 0;
	int32 Popped = 0;
	for (const int32 Id : DugDeposits)
	{
		Popped += Grid->GetDeposit(Id)->PoppedPickups;
		bool bGone = true;
		for (const FIntPoint& Cell : Grid->GetDeposit(Id)->Cells)
		{
			bGone &= !Grid->IsBlockCell(Cell);
		}
		Broken += bGone ? 1 : 0;
	}
	int32 ValueSum = 0;
	const int32 Chunks = CountOrePickups(TestWorld.Get(), ValueSum);
	AddInfo(FString::Printf(TEXT("%d deposit(s) broken, %d ore chunks, value %d"), Broken, Chunks, ValueSum));
	TestTrue(TEXT("4 chunks per broken deposit"), Chunks >= 4 * Broken);
	TestEqual(TEXT("chunks = pickups popped by the dug deposits"), Chunks, Popped);
	TestEqual(TEXT("each chunk is worth 1"), ValueSum, Chunks);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRROreExplosionDropsTest, "RogueRigs.Ore.ExplosionDrops",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRROreExplosionDropsTest::RunTest(const FString& Parameters)
{
	using namespace RROreDepositTests;

	// No start-zone protection: the deposit nearest the spawn is blown up (as an arrival capsule or a barrel would); its
	// chunks are thrown like any explosion drop. Then three deposits broken in one frame are created by the actor pool within
	// 3 frames (task-092-01, replaces the 4-per-frame queue), and reuse collected chunks at once.
	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnOreGrid(*this, TestWorld, /*bPlaceDeposits*/ true);
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	if (!Grid || !TestNotNull(TEXT("pickup subsystem"), Pickups))
	{
		return false;
	}
	TestEqual(TEXT("pickups created within 3 frames"), Pickups->GetSpawnMaxFrames(), 3);
	TArray<int32> Ids = GetTableDepositIds(Grid);
	if (!TestTrue(TEXT("at least 4 deposits"), Ids.Num() >= 4))
	{
		return false;
	}
	const FVector SpawnCenter = Grid->GetCellCenter(Grid->GetSpawnCell().X, Grid->GetSpawnCell().Y);
	Ids.Sort([Grid, SpawnCenter](int32 A, int32 B)
	{
		return FVector::DistSquared2D(GetDepositCenter(Grid, *Grid->GetDeposit(A)), SpawnCenter)
			< FVector::DistSquared2D(GetDepositCenter(Grid, *Grid->GetDeposit(B)), SpawnCenter);
	});
	const FRRGridDeposit& Nearest = *Grid->GetDeposit(Ids[0]);
	AddInfo(FString::Printf(TEXT("nearest deposit to the spawn at %s, %.0f uu away"), *Nearest.Cells[0].ToString(),
		FVector::Dist2D(GetDepositCenter(Grid, Nearest), SpawnCenter)));

	// Radius 0.75 cell around the deposit centre: exactly its 4 blocks (their centres are 0.71 cell away, the next 1.58).
	const int32 Destroyed = Grid->DestroyCellsInRadius(GetDepositCenter(Grid, Nearest), 0.75f * Grid->GetCellSize(), ERRTerrainDamageSource::Barrel);
	TestEqual(TEXT("explosion breaks the 4 blocks"), Destroyed, 4);
	int32 ValueSum = 0;
	TestEqual(TEXT("4 chunk creations queued in the pool"), Pickups->GetQueuedSpawnCount(), 4);
	for (int32 Frame = 0; Frame < 3 && Pickups->GetQueuedSpawnCount() > 0; ++Frame)
	{
		TestWorld.Tick();
	}
	TestEqual(TEXT("4 chunks thrown within 3 frames"), CountOrePickups(TestWorld.Get(), ValueSum), 4);
	TestEqual(TEXT("chunks worth 1 each"), ValueSum, 4);
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		// task-092-01: idle pool pickups are not live pickups.
		if (URRActorPoolSubsystem::IsPooledIdle(*It))
		{
			continue;
		}
		TestEqual(TEXT("chunk pops from the grid"), It->GetPhase(), ERRPickupPhase::Pop);
	}

	// Next frame: three more deposits in one frame = 12 chunks, created by the pool over 3 frames (ceil(12 / 3) = 4 per frame).
	TestWorld.Tick();
	for (int32 Index = 1; Index <= 3; ++Index)
	{
		for (const FIntPoint& Cell : Grid->GetDeposit(Ids[Index])->Cells)
		{
			Grid->DestroyCell(Cell, ERRTerrainDamageSource::Kamikaze);
		}
	}
	TestEqual(TEXT("12 queued"), Pickups->GetQueuedSpawnCount(), 12);
	int32 MaxCreatedPerFrame = 0;
	int32 Frames = 0;
	while (Pickups->GetQueuedSpawnCount() > 0 && Frames < 10)
	{
		TestWorld.Tick();
		++Frames;
		MaxCreatedPerFrame = FMath::Max(MaxCreatedPerFrame, Pickups->GetPoolStats().CreatedThisFrame);
	}
	TestTrue(TEXT("12 chunks created within 3 frames"), Frames <= 3);
	TestEqual(TEXT("at most 4 created per frame"), MaxCreatedPerFrame, 4);
	TestEqual(TEXT("16 chunks on the floor"), CountOrePickups(TestWorld.Get(), ValueSum), 16);
	TestEqual(TEXT("queue empty"), Pickups->GetQueuedSpawnCount(), 0);
	TestEqual(TEXT("16 chunks worth 16"), ValueSum, 16);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRROreHealVeinTest, "RogueRigs.Ore.HealVein",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRROreHealVeinTest::RunTest(const FString& Parameters)
{
	using namespace RROreDepositTests;

	// heal-ore (owner 2026-10-10): 12 veins of 2 x 2 (48 cells), each one deposit; HealValue HP per pickup (OreTypes.csv row Heal, 30),
	// so a vein restores 4 x HealValue (120). Every heal pickup is collected, even those that pop inside the pickup radius.
	FRROreTypeRow HealRow;
	if (!TestTrue(TEXT("row Heal found in OreTypes.csv"), ARRArenaGrid::GetOreType(HealId, HealRow)))
	{
		return false;
	}
	TestEqual(TEXT("row Heal is the HealOre block type"), HealRow.CellType, ERRCellType::HealOre);
	TestNearlyEqual(TEXT("row Heal never placed on rock (veins come from the layout)"), HealRow.RockShare, 0.f);
	TestTrue(TEXT("row Heal: HealValue > 0 HP per pickup"), HealRow.HealValue > 0.f);
	TestTrue(TEXT("row Heal writes no colour (alpha 0)"), HealRow.Color.A <= 0.f);

	FTestWorld TestWorld;
	ARRArenaGrid* Grid = SpawnOreGrid(*this, TestWorld, /*bPlaceDeposits*/ false);
	UClass* HealClass = LoadTestClass<ARRPickup>(*this, HealPickupClassPath);
	if (!Grid || !HealClass)
	{
		return false;
	}
	if (FGridAccess::HealClass(Grid) != HealClass)
	{
		FGridAccess::HealClass(Grid) = HealClass;
	}

	// 12 heal deposits, each a 2 x 2 square, 48 cells in all.
	int32 Veins = 0;
	int32 VeinCells = 0;
	int32 TargetId = INDEX_NONE;
	for (int32 Id = 0; Id < Grid->GetDepositCount(); ++Id)
	{
		const FRRGridDeposit& Deposit = *Grid->GetDeposit(Id);
		if (Deposit.CellType != ERRCellType::HealOre)
		{
			continue;
		}
		++Veins;
		VeinCells += Deposit.Cells.Num();
		FIntPoint Min(INT32_MAX, INT32_MAX);
		FIntPoint Max(INT32_MIN, INT32_MIN);
		for (const FIntPoint& Cell : Deposit.Cells)
		{
			Min = FIntPoint(FMath::Min(Min.X, Cell.X), FMath::Min(Min.Y, Cell.Y));
			Max = FIntPoint(FMath::Max(Max.X, Cell.X), FMath::Max(Max.Y, Cell.Y));
		}
		TestTrue(FString::Printf(TEXT("heal vein at %s is a 2 x 2 square"), *Min.ToString()), Deposit.Cells.Num() == 4 && Max - Min == FIntPoint(1, 1));
		TestTrue(FString::Printf(TEXT("heal vein at %s: row Heal, its HealValue, no colour"), *Min.ToString()),
			Deposit.OreId == HealId && FMath::IsNearlyEqual(Deposit.HealValue, HealRow.HealValue) && !Deposit.bHasColor);
		if (Deposit.Cells.Contains(HealVeinCell))
		{
			TargetId = Id;
		}
	}
	TestEqual(TEXT("12 heal veins"), Veins, 12);
	TestEqual(TEXT("48 heal vein cells"), VeinCells, 48);
	if (!TestTrue(TEXT("(148, 136) is in a heal vein"), TargetId != INDEX_NONE))
	{
		return false;
	}
	const TArray<FIntPoint> VeinCellsOfTarget = Grid->GetDeposit(TargetId)->Cells;
	TArray<FIntPoint> FromOneBlock;
	Grid->GetDepositCells(HealVeinCell, FromOneBlock);
	TestEqual(TEXT("touching one block reaches the 4 blocks of its vein"), FromOneBlock.Num(), 4);

	// Miner north of the vein, damaged, possessed (pickups look for the player 0 pawn), weapons silent.
	ARRMiner* Miner = SpawnMiner(*this, TestWorld, Grid, HealVeinNorthFloorCell);
	ARRDrone* Drone = Miner ? Miner->GetDrone() : nullptr;
	URRAbilitySystemComponent* ASC = Miner ? Miner->GetRRAbilitySystemComponent() : nullptr;
	APlayerController* Controller = Miner ? TestWorld.Spawn<APlayerController>(APlayerController::StaticClass(), Miner->GetActorLocation()) : nullptr;
	if (!TestNotNull(TEXT("BP_Miner spawned its drone"), Drone) || !TestNotNull(TEXT("miner ASC"), ASC) || !TestNotNull(TEXT("player controller"), Controller))
	{
		return false;
	}
	Controller->Possess(Miner);
	for (ARRWeapon* Weapon : Miner->GetWeapons())
	{
		if (IsValid(Weapon) && Weapon->GetAutoFireComponent())
		{
			Weapon->GetAutoFireComponent()->StopFiring();
		}
	}
	// Max 200: the 4 pickups of the vein (4 x 30 = 120 since task-490) all heal without reaching the cap.
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetMaxHealthAttribute(), 200.f);
	ASC->SetNumericAttributeBase(URRHealthAttributeSet::GetHealthAttribute(), 10.f);
	TStrongObjectPtr<URRTestEventRecorder> Recorder(NewObject<URRTestEventRecorder>());
	Miner->OnPickupCollected.AddDynamic(Recorder.Get(), &URRTestEventRecorder::HandlePickupCollected);

	// Dig: the 4 blocks shake together and break on the same frame, about 2 s in; the 4 heal pickups worth HealValue pop one by one
	// during the dig, every 0.5 s, the last on the break frame (task-235-01).
	// task-213-02: each pickup pops at a random yaw and 80-300 uu (task-195-02) from the global FMath stream, clock-seeded; a
	// pickup popping away from the miner can land beyond the pickup radius and stay there. Seeded only so the logged numbers
	// repeat; since task-250-01 the result no longer depends on the seed (the miner walks to a pickup left beyond the radius).
	FMath::RandInit(5);
	TArray<int32> BreakFrames;
	BreakFrames.Init(INDEX_NONE, VeinCellsOfTarget.Num());
	int32 MaxVeinBlocksInArc = 0;
	bool bAllShookTogether = false;
	int32 HealPickupsSeen = 0;
	TSet<TWeakObjectPtr<ARRPickup>> Seen;
	TArray<int32> PopFrames;
	for (int32 Frame = 1; Frame <= 180; ++Frame)
	{
		TestWorld.Tick();
		while (PopFrames.Num() < Grid->GetDeposit(TargetId)->PoppedPickups)
		{
			PopFrames.Add(Frame);
		}
		int32 InArc = 0;
		for (const FIntPoint& Cell : Drone->GetDigCells())
		{
			InArc += VeinCellsOfTarget.Contains(Cell) ? 1 : 0;
		}
		MaxVeinBlocksInArc = FMath::Max(MaxVeinBlocksInArc, InArc);
		if (Frame == 30)
		{
			bAllShookTogether = true;
			for (const FIntPoint& Cell : VeinCellsOfTarget)
			{
				bAllShookTogether &= FGridAccess::Shaking(Grid).Contains(Cell);
			}
		}
		for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
		{
			// task-092-01: idle pool pickups are not live pickups. task-259-01: with the 2 s dig the first pickup is collected
			// before the last pops, and the pool hands its actor back for the last one: an idle actor leaves Seen, so its next
			// life counts as a new pickup.
			if (URRActorPoolSubsystem::IsPooledIdle(*It))
			{
				Seen.Remove(*It);
				continue;
			}
			if (It->GetPickupType() == ERRPickupType::Heal && !Seen.Contains(*It))
			{
				Seen.Add(*It);
				++HealPickupsSeen;
				TestNearlyEqual(TEXT("heal pickup worth the row Heal HealValue"), It->GetHealAmount(), HealRow.HealValue);
			}
		}
		bool bAllBroken = true;
		for (int32 Index = 0; Index < VeinCellsOfTarget.Num(); ++Index)
		{
			if (BreakFrames[Index] == INDEX_NONE && !Grid->IsBlockCell(VeinCellsOfTarget[Index]))
			{
				BreakFrames[Index] = Frame;
			}
			bAllBroken &= BreakFrames[Index] != INDEX_NONE;
		}
		if (bAllBroken && HealPickupsSeen >= 4)
		{
			break;
		}
	}
	AddInfo(FString::Printf(TEXT("vein blocks in the arc at most %d; break frames %d %d %d %d"), MaxVeinBlocksInArc,
		BreakFrames[0], BreakFrames[1], BreakFrames[2], BreakFrames[3]));
	TestTrue(TEXT("the drone arc touched the vein"), MaxVeinBlocksInArc >= 1);
	TestTrue(TEXT("the 4 blocks shake together while mined"), bAllShookTogether);
	for (int32 Index = 0; Index < VeinCellsOfTarget.Num(); ++Index)
	{
		TestEqual(FString::Printf(TEXT("block %s breaks with the first"), *VeinCellsOfTarget[Index].ToString()), BreakFrames[Index], BreakFrames[0]);
	}
	TestTrue(TEXT("the vein breaks after about 2 s"), BreakFrames[0] >= 119 && BreakFrames[0] <= 121);
	CheckPopTiming(*this, PopFrames, BreakFrames[0]);
	TestEqual(TEXT("4 heal pickups"), HealPickupsSeen, 4);

	// Miner standing still: every heal pickup that settles inside the pickup radius is collected (task-250-01).
	for (int32 Frame = 0; Frame < 600 && Recorder->GetCollectedPickups().Num() < 4; ++Frame)
	{
		TestWorld.Tick();
	}
	const int32 CollectedInPlace = Recorder->GetCollectedPickups().Num();

	// task-250-01: a pickup pops 80-300 uu (task-195-02) from its own block (task-235-01), up to 2 cells from the miner, so it
	// can settle beyond the 300 uu pickup radius (heal-ore: collected within the radius only). Such a pickup must stay on the
	// floor, at rest, never consumed, and be collected once the miner walks near it (miner teleported to the nearest floor cell).
	TArray<TWeakObjectPtr<ARRPickup>> LeftPickups;
	for (TActorIterator<ARRPickup> It(TestWorld.Get()); It; ++It)
	{
		if (!URRActorPoolSubsystem::IsPooledIdle(*It) && It->GetPickupType() == ERRPickupType::Heal)
		{
			const float LeftDistance = FVector::Dist2D(It->GetActorLocation(), Miner->GetActorLocation());
			AddInfo(FString::Printf(TEXT("heal pickup left by the still miner %.1f uu away (pickup radius %.1f), phase %d"),
				LeftDistance, Miner->GetPickupRadius(), static_cast<int32>(It->GetPhase())));
			TestTrue(TEXT("a pickup left by the still miner rests beyond the pickup radius"),
				It->GetPhase() == ERRPickupPhase::Rest && LeftDistance > Miner->GetPickupRadius());
			LeftPickups.Add(*It);
		}
	}
	TestEqual(TEXT("every heal pickup is collected or still in the world (none lost)"), CollectedInPlace + LeftPickups.Num(), 4);
	int32 Walks = 0;
	for (const TWeakObjectPtr<ARRPickup>& LeftPtr : LeftPickups)
	{
		ARRPickup* Left = LeftPtr.Get();
		if (!Left || URRActorPoolSubsystem::IsPooledIdle(Left))
		{
			continue; // collected meanwhile (pulled in on the way to another one)
		}
		const FIntPoint LeftCell = Grid->GetCellAtWorld(Left->GetActorLocation());
		FIntPoint WalkCell(INDEX_NONE, INDEX_NONE);
		float BestDistance = TNumericLimits<float>::Max();
		for (int32 DY = -3; DY <= 3; ++DY)
		{
			for (int32 DX = -3; DX <= 3; ++DX)
			{
				const FIntPoint Cell = LeftCell + FIntPoint(DX, DY);
				const float Distance = FVector::Dist2D(Grid->GetCellCenter(Cell.X, Cell.Y), Left->GetActorLocation());
				if (Grid->GetCellType(Cell) == ERRCellType::Floor && Distance < BestDistance)
				{
					BestDistance = Distance;
					WalkCell = Cell;
				}
			}
		}
		if (!TestTrue(TEXT("a floor cell next to the left pickup"), WalkCell.X != INDEX_NONE))
		{
			break;
		}
		const FVector WalkCenter = Grid->GetCellCenter(WalkCell.X, WalkCell.Y);
		Miner->SetActorLocation(FVector(WalkCenter.X, WalkCenter.Y, Miner->GetActorLocation().Z), false, nullptr, ETeleportType::TeleportPhysics);
		++Walks;
		auto IsGone = [&LeftPtr]() { return !LeftPtr.IsValid() || URRActorPoolSubsystem::IsPooledIdle(LeftPtr.Get()); };
		for (int32 Frame = 0; Frame < 300 && !IsGone(); ++Frame)
		{
			TestWorld.Tick();
		}
		TestTrue(TEXT("a pickup left beyond the radius is collected once the miner is near it"), IsGone());
	}
	AddInfo(FString::Printf(TEXT("heal pickups collected in place %d, after walking to the rest %d (walks %d)"), CollectedInPlace,
		Recorder->GetCollectedPickups().Num() - CollectedInPlace, Walks));
	int32 HealCollected = 0;
	for (const ERRPickupType Type : Recorder->GetCollectedPickups())
	{
		HealCollected += Type == ERRPickupType::Heal ? 1 : 0;
	}
	TestEqual(TEXT("the 4 heal pickups are collected"), HealCollected, 4);
	TestNearlyEqual(TEXT("the vein restored 4 x HealValue (10 -> 130 of 200)"), ASC->GetNumericAttribute(URRHealthAttributeSet::GetHealthAttribute()),
		10.f + 4.f * HealRow.HealValue, 0.01f);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
