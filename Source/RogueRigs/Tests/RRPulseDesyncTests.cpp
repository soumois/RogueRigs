// Rogue Rigs - headless check of the seeded pulse desync (RogueRigs.PulseDesync.SeededValues, task-257-02): hash determinism, ore and heal pickup Custom Primitive Data, XP neutral, deposit crystal instance data, heal deposit rock shake inputs still (task-258-01).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Pickups/RRPickup.h"
#include "Pickups/RRPickupSubsystem.h"
#include "Pickups/RRPulseDesync.h"
#include "Pool/RRActorPoolSubsystem.h"
#include "Tests/RRTestAccess.h"
#include "Tests/RRTestHarness.h"

namespace RRPulseDesyncTests
{
	using RRTestHarness::FTestWorld;

	/** Pickup Blueprints of the game (ore and heal: desynchronised; XP: neutral). */
	const TCHAR* const OrePickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupOre.BP_PickupOre_C");
	const TCHAR* const HealPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupHeal.BP_PickupHeal_C");
	const TCHAR* const XPPickupClassPath = TEXT("/Game/RogueRigs/Blueprints/Pickups/BP_PickupXP.BP_PickupXP_C");

	/** Value the material sees at Custom Primitive Data Index of Pickup's root mesh: the written float, or Default when never written. */
	float ReadPrimitiveData(const ARRPickup* Pickup, int32 Index, float Default)
	{
		const UPrimitiveComponent* Mesh = Pickup ? Cast<UPrimitiveComponent>(Pickup->GetRootComponent()) : nullptr;
		const TArray<float>& Data = Mesh ? Mesh->GetCustomPrimitiveData().Data : TArray<float>();
		return Data.IsValidIndex(Index) ? Data[Index] : Default;
	}

	/** Per-instance custom data float Index of Instance (-1 when unreadable). */
	float ReadInstanceData(const UInstancedStaticMeshComponent* Component, int32 Instance, int32 Index)
	{
		const int32 Flat = Instance * Component->NumCustomDataFloats + Index;
		return Index < Component->NumCustomDataFloats && Component->PerInstanceSMCustomData.IsValidIndex(Flat) ? Component->PerInstanceSMCustomData[Flat] : -1.f;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRPulseDesyncSeededValuesTest, "RogueRigs.PulseDesync.SeededValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRPulseDesyncSeededValuesTest::RunTest(const FString& Parameters)
{
	using namespace RRPulseDesyncTests;
	using RRTestAccess::FGridAccess;
	using RRTestAccess::FPickupAccess;

	// 1. Hash: two runs of the same seed and id give the same values; phases in [0, 1), rates in [0.8, 1.2].
	int32 Mismatches = 0;
	int32 OutOfRange = 0;
	float MinRate = 10.f;
	float MaxRate = 0.f;
	for (const uint32 Domain : { FRRPulseDesync::PickupDomain, FRRPulseDesync::DepositDomain, FRRPulseDesync::CellDomain })
	{
		for (uint32 Id = 0; Id < 2000; ++Id)
		{
			const float Phase = FRRPulseDesync::Phase(1, Domain, Id);
			const float Rate = FRRPulseDesync::Rate(1, Domain, Id, 0.8f, 1.2f);
			Mismatches += (Phase != FRRPulseDesync::Phase(1, Domain, Id) || Rate != FRRPulseDesync::Rate(1, Domain, Id, 0.8f, 1.2f)) ? 1 : 0;
			OutOfRange += (Phase < 0.f || Phase >= 1.f || Rate < 0.8f || Rate > 1.2f) ? 1 : 0;
			MinRate = FMath::Min(MinRate, Rate);
			MaxRate = FMath::Max(MaxRate, Rate);
		}
	}
	AddInfo(FString::Printf(TEXT("hash: 6000 draws, rate min %.4f max %.4f"), MinRate, MaxRate));
	TestEqual(TEXT("same seed and id give the same phase and rate"), Mismatches, 0);
	TestEqual(TEXT("phases in [0, 1) and rates in [0.8, 1.2]"), OutOfRange, 0);
	TestNotEqual(TEXT("another seed gives another phase"), FRRPulseDesync::Phase(2, FRRPulseDesync::PickupDomain, 0), FRRPulseDesync::Phase(1, FRRPulseDesync::PickupDomain, 0));

	FTestWorld TestWorld(TEXT("RRPulseDesync"));
	URRPickupSubsystem* Pickups = TestWorld.Get()->GetSubsystem<URRPickupSubsystem>();
	UClass* OreClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, RRPulseDesyncTests::OrePickupClassPath);
	UClass* HealClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, RRPulseDesyncTests::HealPickupClassPath);
	UClass* XPClass = RRTestHarness::LoadTestClass<ARRPickup>(*this, RRPulseDesyncTests::XPPickupClassPath);
	if (!TestNotNull(TEXT("pickup subsystem"), Pickups) || !OreClass || !HealClass || !XPClass)
	{
		return false;
	}
	Pickups->SetSpawnMaxFrames(0);

	// 2. Pickups: each Ore and Heal life writes the hash of (PulseSeed, its spawn id); two spawn ids give two phases.
	auto CheckPickup = [this](const ARRPickup* Pickup, const TCHAR* Name)
	{
		const int32 Id = Pickup->GetPulseSpawnId();
		if (!TestTrue(FString::Printf(TEXT("%s has a pulse spawn id"), Name), Id != INDEX_NONE))
		{
			return -1.f;
		}
		const int32 Seed = FPickupAccess::PulseSeedOf(Pickup);
		const FVector2f Range = FPickupAccess::PulseRateRangeOf(Pickup);
		const float Phase = ReadPrimitiveData(Pickup, FPickupAccess::PulsePhaseIndexOf(Pickup), -1.f);
		const float Rate = ReadPrimitiveData(Pickup, FPickupAccess::PulseRateIndexOf(Pickup), -1.f);
		AddInfo(FString::Printf(TEXT("%s: seed %d, spawn id %d, phase %.4f, rate %.4f"), Name, Seed, Id, Phase, Rate));
		TestEqual(FString::Printf(TEXT("%s phase = hash of its seed and id"), Name), Phase, FRRPulseDesync::Phase(Seed, FRRPulseDesync::PickupDomain, Id));
		TestEqual(FString::Printf(TEXT("%s rate = hash of its seed and id"), Name), Rate, FRRPulseDesync::Rate(Seed, FRRPulseDesync::PickupDomain, Id, Range.X, Range.Y));
		TestTrue(FString::Printf(TEXT("%s rate in [0.8, 1.2]"), Name), Rate >= 0.8f && Rate <= 1.2f);
		return Phase;
	};
	const FVector Origin(0.f, 0.f, 100.f);
	ARRPickup* OreA = Pickups->RequestSpawn(OreClass, Origin, nullptr, 1);
	ARRPickup* OreB = Pickups->RequestSpawn(OreClass, Origin + FVector(300.f, 0.f, 0.f), nullptr, 1);
	ARRPickup* Heal = Pickups->RequestSpawn(HealClass, Origin + FVector(0.f, 300.f, 0.f), nullptr, 0, 15.f);
	// Spawned directly: RequestXPCube defers BP_PickupXP to the big block merge (XPBigPickupClass set).
	ARRPickup* XP = TestWorld.Spawn<ARRPickup>(XPClass, Origin + FVector(300.f, 300.f, 0.f));
	if (!TestNotNull(TEXT("ore pickup A"), OreA) || !TestNotNull(TEXT("ore pickup B"), OreB) || !TestNotNull(TEXT("heal pickup"), Heal) || !TestNotNull(TEXT("XP pickup"), XP))
	{
		return false;
	}
	const float PhaseA = CheckPickup(OreA, TEXT("ore A"));
	const float PhaseB = CheckPickup(OreB, TEXT("ore B"));
	CheckPickup(Heal, TEXT("heal"));
	TestEqual(TEXT("consecutive spawn ids"), OreB->GetPulseSpawnId(), OreA->GetPulseSpawnId() + 1);
	TestNotEqual(TEXT("two ore pickups of different spawn ids get different phases"), PhaseA, PhaseB);

	// XP: never written, so the material sees its defaults (phase 0, rate 1).
	TestEqual(TEXT("XP pickup has no pulse spawn id"), XP->GetPulseSpawnId(), static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("XP pickup phase neutral (0)"), ReadPrimitiveData(XP, 1, 0.f), 0.f);
	TestEqual(TEXT("XP pickup rate neutral (1)"), ReadPrimitiveData(XP, 2, 1.f), 1.f);

	// Pool: a released ore pickup is back to neutral; reused, it draws a new spawn id and writes its values.
	URRActorPoolSubsystem* Pool = TestWorld.Get()->GetSubsystem<URRActorPoolSubsystem>();
	if (TestNotNull(TEXT("actor pool"), Pool) && TestTrue(TEXT("ore A released to the pool"), Pool->Release(OreA)))
	{
		TestEqual(TEXT("released ore phase neutral"), ReadPrimitiveData(OreA, FPickupAccess::PulsePhaseIndexOf(OreA), -1.f), 0.f);
		TestEqual(TEXT("released ore rate neutral"), ReadPrimitiveData(OreA, FPickupAccess::PulseRateIndexOf(OreA), -1.f), 1.f);
		ARRPickup* Reused = Pickups->RequestSpawn(OreClass, Origin, nullptr, 1);
		if (TestTrue(TEXT("the released ore pickup is reused"), Reused == OreA))
		{
			TestEqual(TEXT("reuse draws the next spawn id"), Reused->GetPulseSpawnId(), Heal->GetPulseSpawnId() + 1);
			CheckPickup(Reused, TEXT("reused ore"));
		}
	}

	// 3. Deposit crystals (BP_ArenaGrid, arena_m1_fine): crystal-only meshes (Duskite) 6 floats, hash of (PulseSeed, deposit id);
	// meshes with a rock slot (heal) 4 floats, no pulse, every rock shake input below 1 (task-258-01).
	ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	const int32 Seed = FGridAccess::PulseSeedOf(Grid);
	const FVector2f Range = FGridAccess::PulseRateRangeOf(Grid);
	const int32 PhaseIndex = FGridAccess::PulsePhaseIndexOf(Grid);
	const int32 RateIndex = FGridAccess::PulseRateIndexOf(Grid);
	TestEqual(TEXT("crystal phase index 4"), PhaseIndex, 4);
	TestEqual(TEXT("crystal rate index 5"), RateIndex, 5);
	int32 Checked = 0;
	int32 Wrong = 0;
	int32 RockChecked = 0;
	int32 RockShaking = 0;
	int32 RockWouldShake = 0;
	for (const FRRGridDepositVisual& Visual : FGridAccess::DepositVisualsOf(Grid))
	{
		if (!Visual.Render)
		{
			continue;
		}
		// task-258-01: a mesh with a rock slot (heal deposit, M_RockSlab) shares the per-instance data with the rock, which reads
		// 4-7 as its quadrant shake (1 or more = shaking). Such an ISM gets no pulse data and 4 floats, so every shake input reads
		// its default 0 (still); the rate task-257-02 wrote at 5 is counted to show how many deposits shook.
		if (FGridAccess::DepositMeshHasRockSlotOf(Grid, Visual.Render->GetStaticMesh()))
		{
			TestFalse(TEXT("a deposit ISM with a rock slot carries no pulse data"), Visual.bPulseData);
			TestEqual(TEXT("a deposit ISM with a rock slot has 4 custom data floats (marked, colour)"), Visual.Render->NumCustomDataFloats, 4);
			for (int32 Instance = 0; Instance < Visual.InstanceDeposits.Num(); ++Instance)
			{
				const uint32 DepositId = static_cast<uint32>(Visual.InstanceDeposits[Instance]);
				float MaxShake = 0.f;
				for (int32 Quadrant = 0; Quadrant < 4; ++Quadrant)
				{
					// Index beyond NumCustomDataFloats: the material reads its default 0.
					const int32 Index = ARRArenaGrid::PieceShakeCustomDataIndex + Quadrant;
					MaxShake = FMath::Max(MaxShake, Index < Visual.Render->NumCustomDataFloats ? ReadInstanceData(Visual.Render, Instance, Index) : 0.f);
				}
				const float FormerRate = FRRPulseDesync::Rate(Seed, FRRPulseDesync::DepositDomain, DepositId, Range.X, Range.Y);
				AddInfo(FString::Printf(TEXT("rock-slot deposit %u: rock shake inputs max %.4f (still below 1); task-257-02 rate at index 5 was %.4f (%s)"),
					DepositId, MaxShake, FormerRate, FormerRate >= 1.f ? TEXT("shook quadrant 1") : TEXT("still")));
				++RockChecked;
				RockShaking += MaxShake >= 1.f ? 1 : 0;
				RockWouldShake += FormerRate >= 1.f ? 1 : 0;
			}
			continue;
		}
		TestTrue(TEXT("a crystal-only deposit ISM carries the pulse data"), Visual.bPulseData);
		TestEqual(TEXT("crystal ISM has 6 custom data floats"), Visual.Render->NumCustomDataFloats, 6);
		for (int32 Instance = 0; Instance < Visual.InstanceDeposits.Num(); ++Instance)
		{
			const uint32 DepositId = static_cast<uint32>(Visual.InstanceDeposits[Instance]);
			const float Phase = ReadInstanceData(Visual.Render, Instance, PhaseIndex);
			const float Rate = ReadInstanceData(Visual.Render, Instance, RateIndex);
			++Checked;
			if (Phase != FRRPulseDesync::Phase(Seed, FRRPulseDesync::DepositDomain, DepositId)
				|| Rate != FRRPulseDesync::Rate(Seed, FRRPulseDesync::DepositDomain, DepositId, Range.X, Range.Y) || Rate < 0.8f || Rate > 1.2f)
			{
				if (Wrong++ < 5)
				{
					AddError(FString::Printf(TEXT("deposit %u crystal: phase %.4f rate %.4f"), DepositId, Phase, Rate));
				}
			}
		}
	}
	AddInfo(FString::Printf(TEXT("seed %d: %d deposit crystal instances checked over %d ISMs"), Seed, Checked, FGridAccess::DepositVisualsOf(Grid).Num()));
	TestTrue(TEXT("deposit crystals present"), Checked > 1);
	TestEqual(TEXT("every crystal holds the hash of its deposit"), Wrong, 0);
	AddInfo(FString::Printf(TEXT("rock-slot deposits: %d checked, %d with a shake input of 1 or more (task-257-02 layout: %d)"), RockChecked, RockShaking, RockWouldShake));
	TestTrue(TEXT("rock-slot (heal) deposits present"), RockChecked > 0);
	TestEqual(TEXT("no rock-slot deposit has a shake input of 1 or more (untouched deposits stay still)"), RockShaking, 0);

	// Two neighbouring instances (0 and 1) of the first ore crystal ISM: different phases; colour and mark kept. The heal ISMs
	// (rock slot) have no pulse data since task-258-01; their mark is checked too.
	for (const ERRCellType Type : { ERRCellType::Ore, ERRCellType::HealOre })
	{
		const FRRGridDepositVisual* Visual = FGridAccess::DepositVisualsOf(Grid).FindByPredicate(
			[Type](const FRRGridDepositVisual& Candidate) { return Candidate.CellType == Type && Candidate.Render && Candidate.InstanceDeposits.Num() >= 2; });
		if (!TestNotNull(Type == ERRCellType::Ore ? TEXT("an ore crystal ISM with 2 instances") : TEXT("a heal crystal ISM with 2 instances"), Visual))
		{
			continue;
		}
		if (!Visual->bPulseData)
		{
			TestEqual(TEXT("unmarked heal deposit keeps mark 0"), ReadInstanceData(Visual->Render, 0, Grid->GetMarkedCustomDataIndex()), 0.f);
			continue;
		}
		const float First = ReadInstanceData(Visual->Render, 0, PhaseIndex);
		const float Second = ReadInstanceData(Visual->Render, 1, PhaseIndex);
		AddInfo(FString::Printf(TEXT("%s deposits %d and %d: phases %.4f and %.4f, rates %.4f and %.4f"), Type == ERRCellType::Ore ? TEXT("ore") : TEXT("heal"),
			Visual->InstanceDeposits[0], Visual->InstanceDeposits[1], First, Second, ReadInstanceData(Visual->Render, 0, RateIndex), ReadInstanceData(Visual->Render, 1, RateIndex)));
		TestNotEqual(TEXT("two neighbouring crystal instances get different phases"), First, Second);
		TestEqual(TEXT("unmarked crystal keeps mark 0"), ReadInstanceData(Visual->Render, 0, Grid->GetMarkedCustomDataIndex()), 0.f);
	}
	return true;
}

#endif
