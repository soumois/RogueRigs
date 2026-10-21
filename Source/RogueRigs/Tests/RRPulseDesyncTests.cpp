// Rogue Rigs - headless check of the seeded pulse desync (RogueRigs.PulseDesync.SeededValues, task-257-02): hash determinism, ore and heal pickup Custom Primitive Data, XP never written, deposit crystal instance data at 13 / 14 (task-267-01), heal deposit rock shake inputs still (task-258-01).

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Components/InstancedStaticMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameters.h"
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

	/**
	 * task-267-01 evidence: where the M_XPNutGlow / M_DepositCrystalGlow `PulseRate` parameter of Component's materials comes from
	 * (its Custom Primitive Data slot and material default) and the float the GPU gets at that slot. The engine never feeds the
	 * material default into the primitive data: UMaterialExpressionScalarParameter::Compile emits CustomPrimitiveData(index) when
	 * bUseCustomPrimitiveData, and FPrimitiveUniformShaderParametersBuilder::CustomPrimitiveData zeroes an empty array (unwritten
	 * slots of a shorter array are zeroed by SetPrimitiveData's SetNumZeroed). So an unwritten slot reads 0.
	 */
	FString DescribePulseRateSource(const UPrimitiveComponent* Component)
	{
		if (!Component)
		{
			return TEXT("no component");
		}
		const FName PulseRate(TEXT("PulseRate"));
		const int32 Slot = Component->GetCustomPrimitiveDataIndexForScalarParameter(PulseRate);
		float MaterialDefault = -1.f;
		for (int32 Element = 0; Element < Component->GetNumMaterials(); ++Element)
		{
			FMaterialParameterMetadata Meta;
			const UMaterialInterface* Material = Component->GetMaterial(Element);
			if (Material && Material->GetParameterValue(EMaterialParameterType::Scalar, FMemoryImageMaterialParameterInfo(PulseRate), Meta, EMaterialGetParameterValueFlags::CheckAll))
			{
				MaterialDefault = Meta.Value.AsScalar();
				break;
			}
		}
		const TArray<float>& Data = Component->GetCustomPrimitiveData().Data;
		const float GpuValue = Slot >= 0 && Data.IsValidIndex(Slot) ? Data[Slot] : 0.f;
		return FString::Printf(TEXT("PulseRate Custom Primitive Data slot %d, material default %.4f, component data %d float(s) (default data %d), value the GPU gets at the slot %.4f"),
			Slot, MaterialDefault, Data.Num(), Component->GetDefaultCustomPrimitiveData().Data.Num(), GpuValue);
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

	// XP: never written. task-267-01: an unwritten Custom Primitive Data slot reaches the GPU as 0, not as the material
	// parameter default (see DescribePulseRateSource), so the XP pulse rate reads 0 until M_XPNutGlow stops reading PulseRate
	// from Custom Primitive Data; logged, not asserted (asset fix pending).
	TestEqual(TEXT("XP pickup has no pulse spawn id"), XP->GetPulseSpawnId(), static_cast<int32>(INDEX_NONE));
	TestEqual(TEXT("XP pickup phase slot never written (reads 0)"), ReadPrimitiveData(XP, 1, 0.f), 0.f);
	AddInfo(FString::Printf(TEXT("XP pickup mesh: %s"), *DescribePulseRateSource(Cast<UPrimitiveComponent>(XP->GetRootComponent()))));
	AddInfo(FString::Printf(TEXT("ore pickup mesh: %s"), *DescribePulseRateSource(Cast<UPrimitiveComponent>(OreB->GetRootComponent()))));

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

	// 3. Deposit crystals (BP_ArenaGrid, arena_m1_fine), task-267-01 layout: every deposit formation ISM (Duskite crystal-only
	// meshes and heal meshes with a rock slot) carries 15 floats with the hash of (PulseSeed, deposit id) at 13 (phase) and 14
	// (rate); on the heal ISMs the rock piece floats 4-12 (M_RockSlab shake 4-7, marked 8-11, solid mask 12) stay 0 (task-258-01).
	ARRArenaGrid* Grid = RRTestHarness::SpawnFineGrid(*this, TestWorld);
	if (!Grid)
	{
		return false;
	}
	const int32 Seed = FGridAccess::PulseSeedOf(Grid);
	const FVector2f Range = FGridAccess::PulseRateRangeOf(Grid);
	const int32 PhaseIndex = FGridAccess::DepositPulsePhaseIndexOf(Grid);
	const int32 RateIndex = FGridAccess::DepositPulseRateIndexOf(Grid);
	TestEqual(TEXT("deposit crystal phase index 13"), PhaseIndex, 13);
	TestEqual(TEXT("deposit crystal rate index 14"), RateIndex, 14);
	TestEqual(TEXT("per-cell cluster phase index stays 4 (M_XPNutGlow)"), FGridAccess::PulsePhaseIndexOf(Grid), 4);
	TestEqual(TEXT("per-cell cluster rate index stays 5 (M_XPNutGlow)"), FGridAccess::PulseRateIndexOf(Grid), 5);
	int32 Checked = 0;
	int32 Wrong = 0;
	int32 RockChecked = 0;
	int32 RockHashedPhase = 0;
	int32 RockRestNonZero = 0;
	float RockRestMax = 0.f;
	bool bLoggedDepositSource[2] = { false, false };
	for (const FRRGridDepositVisual& Visual : FGridAccess::DepositVisualsOf(Grid))
	{
		if (!Visual.Render)
		{
			continue;
		}
		const bool bRockSlot = FGridAccess::DepositMeshHasRockSlotOf(Grid, Visual.Render->GetStaticMesh());
		if (!bLoggedDepositSource[bRockSlot ? 1 : 0])
		{
			bLoggedDepositSource[bRockSlot ? 1 : 0] = true;
			AddInfo(FString::Printf(TEXT("%s deposit ISM: %s"), bRockSlot ? TEXT("heal (rock slot)") : TEXT("Duskite"), *DescribePulseRateSource(Visual.Render)));
		}
		TestTrue(TEXT("every deposit ISM carries the pulse data"), Visual.bPulseData);
		TestEqual(TEXT("deposit ISM has 15 custom data floats"), Visual.Render->NumCustomDataFloats, 15);
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
			if (!bRockSlot)
			{
				continue;
			}
			// Heal: the rock reads 4-12 with the rock piece layout; all must stay 0 (shake on at 1 or more, marked above 0.5).
			float RestMax = 0.f;
			for (int32 Index = ARRArenaGrid::PieceShakeCustomDataIndex; Index < ARRArenaGrid::PieceCustomDataCount; ++Index)
			{
				RestMax = FMath::Max(RestMax, FMath::Abs(ReadInstanceData(Visual.Render, Instance, Index)));
			}
			AddInfo(FString::Printf(TEXT("heal deposit %u: phase %.4f rate %.4f at 13 / 14, rock inputs 4-12 max %.4f"), DepositId, Phase, Rate, RestMax));
			++RockChecked;
			RockHashedPhase += Phase != 0.f ? 1 : 0;
			RockRestNonZero += RestMax != 0.f ? 1 : 0;
			RockRestMax = FMath::Max(RockRestMax, RestMax);
		}
	}
	AddInfo(FString::Printf(TEXT("seed %d: %d deposit crystal instances checked over %d ISMs"), Seed, Checked, FGridAccess::DepositVisualsOf(Grid).Num()));
	TestTrue(TEXT("deposit crystals present"), Checked > 1);
	TestEqual(TEXT("every crystal holds the hash of its deposit"), Wrong, 0);
	AddInfo(FString::Printf(TEXT("heal (rock-slot) deposits: %d checked, %d with a non-zero hashed phase, rock inputs 4-12 max %.4f"), RockChecked, RockHashedPhase, RockRestMax));
	TestEqual(TEXT("12 heal deposits on arena_m1_fine"), RockChecked, 12);
	TestEqual(TEXT("every heal deposit has a non-zero hashed phase"), RockHashedPhase, RockChecked);
	TestEqual(TEXT("no heal deposit has a non-zero rock input at 4-12 (untouched deposits stay still)"), RockRestNonZero, 0);

	// Two neighbouring instances (0 and 1) of the first ore and heal crystal ISMs: different phases; mark kept.
	for (const ERRCellType Type : { ERRCellType::Ore, ERRCellType::HealOre })
	{
		const FRRGridDepositVisual* Visual = FGridAccess::DepositVisualsOf(Grid).FindByPredicate(
			[Type](const FRRGridDepositVisual& Candidate) { return Candidate.CellType == Type && Candidate.Render && Candidate.InstanceDeposits.Num() >= 2; });
		if (!TestNotNull(Type == ERRCellType::Ore ? TEXT("an ore crystal ISM with 2 instances") : TEXT("a heal crystal ISM with 2 instances"), Visual))
		{
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
