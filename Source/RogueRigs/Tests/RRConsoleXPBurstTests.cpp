// Rogue Rigs - automation test of the console hack XP burst (RogueRigs.Console.HackXPBurst): 3 big chips of 8 XP on console completion, none on the bonus chest.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "EngineUtils.h"
#include "Objectives/RRBonusChest.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Tests/RRTestHarness.h"
#include "Weapons/RRAutoFireComponent.h"
#include "Weapons/RRWeapon.h"

namespace RRConsoleXPBurstTests
{
	/** Miner start cell on arena_m1_fine (same as the RunFlow tests). */
	const FIntPoint MinerCell = RRTestHarness::FineCell(FIntPoint(40, 40));

	/** Console cell, 8.49 cells (569 uu) from MinerCell. */
	const FIntPoint ConsoleCell = RRTestHarness::FineCell(FIntPoint(42, 38));

	/** Half diagonal of the 3 x 3 console mesh footprint at scale 1.361 (about 201 x 172 uu). */
	constexpr float ConsoleHalfDiagonal = 133.f;

	/** Live XP pickups in the world (pool idle pickups excluded). */
	TArray<ARRPickup*> GetLiveXPPickups(UWorld* World)
	{
		TArray<ARRPickup*> Result;
		for (TActorIterator<ARRPickup> It(World); It; ++It)
		{
			if (IsValid(*It) && It->IsLifeStarted() && It->GetPickupType() == ERRPickupType::XP)
			{
				Result.Add(*It);
			}
		}
		return Result;
	}

	void TickFrames(const RRTestHarness::FTestWorld& TestWorld, int32 Frames)
	{
		for (int32 Frame = 0; Frame < Frames; ++Frame)
		{
			TestWorld.Tick();
		}
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRRConsoleHackXPBurstTest, "RogueRigs.Console.HackXPBurst",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRRConsoleHackXPBurstTest::RunTest(const FString& Parameters)
{
	using namespace RRTestHarness;
	using namespace RRConsoleXPBurstTests;

	FTestWorld TestWorld(TEXT("RRConsoleHackXPBurstWorld"));
	ARRArenaGrid* Grid = SpawnFineGrid(*this, TestWorld);
	ARRMiner* Miner = Grid ? SpawnMiner(*this, TestWorld, Grid, MinerCell) : nullptr;
	if (!Miner)
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
	if (URRRunFlowSubsystem* RunFlow = TestWorld.Get()->GetSubsystem<URRRunFlowSubsystem>())
	{
		RunFlow->SetRunFlowEnabled(false);
	}
	UWorld* World = TestWorld.Get();
	const FVector CellCenter = Grid->GetCellCenter(ConsoleCell.X, ConsoleCell.Y);

	// Console: 3 chips of 8 XP on completion, spread 120 degrees, landing clear of the mesh.
	ARRConsole* Console = TestWorld.Spawn<ARRConsole>(ARRConsole::StaticClass(), CellCenter);
	if (!TestNotNull(TEXT("console spawned"), Console))
	{
		return false;
	}
	TestEqual(TEXT("console chip count default"), Console->GetXPBurstChipCount(), 3);
	TestEqual(TEXT("console chip XP default"), Console->GetXPBurstChipXP(), 8.f);
	const int32 XPBefore = GetLiveXPPickups(World).Num();
	Console->InitializeConsole(Grid, Miner, ConsoleCell);
	Console->ForceComplete();
	TickFrames(TestWorld, 6); // pooled chips are created within SpawnMaxFrames (3); well before the 1.2 s magnet delay
	TestEqual(TEXT("console requested 3 chips"), Console->GetXPBurstChipsSpawned(), 3);

	const TArray<ARRPickup*> Chips = GetLiveXPPickups(World);
	TestEqual(TEXT("3 XP chips in the world after the console"), Chips.Num() - XPBefore, 3);
	const FVector Centre = Console->GetActorLocation();
	float TotalXP = 0.f;
	TArray<float> Yaws;
	for (const ARRPickup* Chip : Chips)
	{
		TotalXP += Chip->GetXPAmount();
		TestEqual(TEXT("chip worth 8 nuts"), Chip->GetXPAmount(), 8.f);
		TestTrue(TEXT("chip is the big XP class"), Chip->GetClass()->GetName().Contains(TEXT("PickupXPBig")));
		const FVector2D Start(Chip->GetPopStart() - Centre);
		const FVector2D End(Chip->GetPopEnd() - Centre);
		TestTrue(*FString::Printf(TEXT("chip starts clear of the mesh (%.0f uu)"), Start.Size()), Start.Size() > ConsoleHalfDiagonal);
		TestTrue(*FString::Printf(TEXT("chip lands clear of the mesh (%.0f uu)"), End.Size()), End.Size() > ConsoleHalfDiagonal + 50.f);
		TestTrue(TEXT("chip flies outward"), End.Size() > Start.Size());
		Yaws.Add(FMath::RadiansToDegrees(FMath::Atan2(Start.Y, Start.X)));
		AddInfo(FString::Printf(TEXT("chip %s: start %.0f uu, rest %.0f uu from the console centre, yaw %.1f"), *Chip->GetName(), Start.Size(), End.Size(), Yaws.Last()));
	}
	TestEqual(TEXT("24 XP in all"), TotalXP, 24.f);
	Yaws.Sort();
	for (int32 Index = 0; Index < Yaws.Num(); ++Index)
	{
		const float Next = Index + 1 < Yaws.Num() ? Yaws[Index + 1] : Yaws[0] + 360.f;
		TestNearlyEqual(TEXT("yaws 120 degrees apart"), Next - Yaws[Index], 120.f, 0.5f);
	}

	// Bonus chest: its own reward only, no extra XP chip.
	ARRBonusChest* Chest = TestWorld.Spawn<ARRBonusChest>(ARRBonusChest::StaticClass(), CellCenter);
	if (!TestNotNull(TEXT("chest spawned"), Chest))
	{
		return false;
	}
	TestEqual(TEXT("chest chip count 0"), Chest->GetXPBurstChipCount(), 0);
	const int32 XPBeforeChest = GetLiveXPPickups(World).Num();
	Chest->InitializeConsole(Grid, Miner, ConsoleCell);
	Chest->ForceComplete();
	TickFrames(TestWorld, 6);
	TestTrue(TEXT("chest reward granted"), Chest->WasRewardGranted());
	TestEqual(TEXT("chest requested no chip"), Chest->GetXPBurstChipsSpawned(), 0);
	TestEqual(TEXT("no extra XP pickup after the chest"), GetLiveXPPickups(World).Num(), XPBeforeChest);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
