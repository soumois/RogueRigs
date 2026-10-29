// Rogue Rigs - automation test of the hack XP burst (RogueRigs.Console.HackXPBurst): 3 big chips of 8 XP on console completion; on the bonus chest 3 scale-popping chips at the lid opening (task-309-01), whose reward queues exactly one choice.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "EngineUtils.h"
#include "GameFramework/WorldSettings.h"
#include "Objectives/RRBonusChest.h"
#include "Objectives/RRConsole.h"
#include "Objectives/RRRunFlowSubsystem.h"
#include "Pickups/RRPickup.h"
#include "Player/RRMiner.h"
#include "Progression/RRLevelUpComponent.h"
#include "Progression/RRProgressionComponent.h"
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

	// Console chips never scale-pop (task-309-01: chest burst only).
	for (const ARRPickup* Chip : Chips)
	{
		TestFalse(TEXT("console chip has no scale pop"), Chip->IsSpawnScalePopActive());
		TestEqual(TEXT("console chip scale factor 1"), Chip->GetSpawnScaleFactor(), 1.f);
	}

	// Bonus chest (task-309-01): 3 chips of 8 at the lid opening (LidOpenDelay after the completion), each scale-popping
	// 0 -> 1.3 -> 1; no direct XP; with the XP bar above half it queues exactly one choice and no level-up (owner 2026-10-23: one
	// upgrade, not two). No global time dilation.
	ARRBonusChest* Chest = TestWorld.Spawn<ARRBonusChest>(ARRBonusChest::StaticClass(), CellCenter);
	if (!TestNotNull(TEXT("chest spawned"), Chest))
	{
		return false;
	}
	TestEqual(TEXT("chest chip count 3"), Chest->GetXPBurstChipCount(), 3);
	TestEqual(TEXT("chest chip XP 8"), Chest->GetXPBurstChipXP(), 8.f);
	TestNearlyEqual(TEXT("chest chip pop peak 1.3"), Chest->GetChipScalePop().PeakScale, 1.3f, 0.001f);
	TestNearlyEqual(TEXT("chest chip pop 0.25 s"), Chest->GetChipScalePop().Duration, 0.25f, 0.001f);
	TestNearlyEqual(TEXT("lid delay 0.5 s"), Chest->GetLidOpenDelay(), 0.5f, 0.001f);
	TestNearlyEqual(TEXT("lid closed at 0"), Chest->GetLidAngleAt(0.f), 0.f, 0.001f);
	TestNearlyEqual(TEXT("lid at the peak 115 after 0.4 s"), Chest->GetLidAngleAt(0.4f), 115.f, 0.01f);
	TestNearlyEqual(TEXT("lid held at 115"), Chest->GetLidAngleAt(0.45f), 115.f, 0.01f);
	TestNearlyEqual(TEXT("lid settled at 100"), Chest->GetLidAngleAt(0.62f), 100.f, 0.01f);
	TestNearlyEqual(TEXT("latch at 90 after 0.2 s"), Chest->GetLatchAngleAt(0.2f), 90.f, 0.01f);
	const float DilationBefore = World->GetWorldSettings() ? World->GetWorldSettings()->TimeDilation : 1.f;
	const int32 XPBeforeChest = GetLiveXPPickups(World).Num();
	URRProgressionComponent* Progression = Miner->GetProgression();
	URRLevelUpComponent* LevelUp = Miner->GetLevelUp();
	if (!TestNotNull(TEXT("miner progression"), Progression) || !TestNotNull(TEXT("miner level-up"), LevelUp))
	{
		return false;
	}
	Progression->RestoreProgress(Progression->GetLevel(), 0.75f * Progression->GetXPToNextLevel());
	const int32 LevelBeforeChest = Progression->GetLevel();
	const float BarBeforeChest = Progression->GetXP();
	const int32 ChoicesBeforeChest = LevelUp->GetPendingChoices();
	Chest->InitializeConsole(Grid, Miner, ConsoleCell);
	Chest->ForceComplete();
	TestTrue(TEXT("chest opening"), Chest->IsOpening());
	TickFrames(TestWorld, 6); // 0.1 s: the latch swings, the lid waits
	TestFalse(TEXT("no reward before the lid delay"), Chest->WasRewardGranted());
	TestFalse(TEXT("lid not opened before the delay"), Chest->IsLidOpened());
	TestEqual(TEXT("no chip before the lid delay"), Chest->GetXPBurstChipsSpawned(), 0);
	TestEqual(TEXT("no XP pickup before the lid delay"), GetLiveXPPickups(World).Num(), XPBeforeChest);
	TestTrue(TEXT("latch swinging"), Chest->GetLatchAngle() > 0.f);
	TestEqual(TEXT("lid still closed"), Chest->GetLidAngle(), 0.f);
	int32 Frames = 6;
	while (!Chest->IsLidOpened() && Frames < 120)
	{
		TestWorld.Tick();
		++Frames;
	}
	TestTrue(TEXT("lid opened"), Chest->IsLidOpened());
	TestTrue(*FString::Printf(TEXT("lid opened after the 0.5 s delay (%.3f s)"), Chest->GetOpeningElapsed()), Chest->GetOpeningElapsed() >= 0.5f - UE_KINDA_SMALL_NUMBER);
	TestTrue(TEXT("chest reward granted at the lid opening"), Chest->WasRewardGranted());
	TestEqual(TEXT("chest requested 3 chips"), Chest->GetXPBurstChipsSpawned(), 3);
	// Read at the lid opening, before a chip could reach the miner or the choice delay could open the screen.
	TestEqual(TEXT("chest XP fraction default 0"), Chest->GetXPNeedFraction(), 0.f);
	TestEqual(TEXT("chest gives no direct XP"), Chest->GetRewardXP(), 0.f);
	TestEqual(TEXT("no level gained from the chest"), Progression->GetLevel(), LevelBeforeChest);
	TestEqual(TEXT("XP bar unchanged by the chest"), Progression->GetXP(), BarBeforeChest);
	TestEqual(TEXT("chest queues exactly one choice"), LevelUp->GetPendingChoices() - ChoicesBeforeChest, 1);
	AddInfo(FString::Printf(TEXT("chest at bar %.1f / %.1f: pending choices %d -> %d, level %d"), BarBeforeChest, Progression->GetXPToNextLevel(),
		ChoicesBeforeChest, LevelUp->GetPendingChoices(), Progression->GetLevel()));
	TickFrames(TestWorld, 4); // pooled chips are created within SpawnMaxFrames (3)
	TArray<ARRPickup*> ChestChips;
	for (ARRPickup* Chip : GetLiveXPPickups(World))
	{
		// The chest chips start XPBurstSpawnRadius (80 uu) from the centre, the console ones 140 uu.
		const float StartDistance = FVector::Dist2D(Chip->GetPopStart(), Chest->GetActorLocation());
		if (FMath::IsNearlyEqual(StartDistance, Chest->GetXPBurstSpawnRadius(), 1.f))
		{
			ChestChips.Add(Chip);
		}
	}
	TestEqual(TEXT("3 chest chips in the world"), ChestChips.Num(), 3);
	for (const ARRPickup* Chip : ChestChips)
	{
		TestEqual(TEXT("chest chip worth 8 nuts"), Chip->GetXPAmount(), 8.f);
		TestTrue(TEXT("chest chip is the big XP class"), Chip->GetClass()->GetName().Contains(TEXT("PickupXPBig")));
		AddInfo(FString::Printf(TEXT("chest chip %s: scale factor %.3f, pop active %d"), *Chip->GetName(), Chip->GetSpawnScaleFactor(), Chip->IsSpawnScalePopActive() ? 1 : 0));
		TestTrue(TEXT("chest chip scale pop running"), Chip->IsSpawnScalePopActive());
	}
	TickFrames(TestWorld, 20); // past 0.25 s
	for (const ARRPickup* Chip : ChestChips)
	{
		TestFalse(TEXT("chest chip scale pop over"), Chip->IsSpawnScalePopActive());
		TestEqual(TEXT("chest chip back to scale factor 1"), Chip->GetSpawnScaleFactor(), 1.f);
	}
	TickFrames(TestWorld, 240); // light on 3 s, fade 0.5 s
	TestNearlyEqual(TEXT("lid rests at 100"), Chest->GetLidAngle(), 100.f, 0.01f);
	TestNearlyEqual(TEXT("latch rests at 90"), Chest->GetLatchAngle(), 90.f, 0.01f);
	TestFalse(TEXT("opening finished"), Chest->IsOpening());
	TestEqual(TEXT("no global time dilation"), World->GetWorldSettings() ? World->GetWorldSettings()->TimeDilation : 1.f, DilationBefore);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
