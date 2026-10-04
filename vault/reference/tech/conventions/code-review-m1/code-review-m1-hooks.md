---
id: code-review-m1-hooks
description: "M1 review: test-only hooks and cheats inside gameplay classes, test-code readability, dead code, files over 800 lines, oversized docs."
parent: code-review-m1
kind: doc
---
# Review findings — test hooks, cheats, dead code

## Test-only hooks in gameplay classes
- Read-only counters (harmless, keep; group under a `//~ Debug and tests` header section): `ARRChaser::GetAttackCount`, `GetLastAttackTime`; `ARRKamikaze::GetArmTime`, `GetExplosionQueuedTime`, `GetCoreScaleFactor`; `ARRShotgun::GetWaveCount` (`GetLastWave` also feeds `OnWaveFired`); `URRFlowFieldSubsystem::GetComputeCount`.
- Setters public for tests: `ARREnemy::SetIgnoreOtherRobots`, `SetFarMovement`; `URRRunFlowSubsystem::SetConfig`, `SetRandomSeed`, `SetRunFlowEnabled`. Acceptable; say so in the comment.
- Test-sensitive branch: `ARREnemy::UpdateMovementLOD` runs only with a game viewport, so headless tests never exercise far movement. Documented; keep.
- `ARRTestPickupXP` (Tests/) is a UCLASS compiled in every configuration. Documented, `HideDropdown`; keep.

## Cheats inside gameplay classes
- H1 `URRRunFlowSubsystem::SkipToConsole`, `CompleteConsoleHack`, `SkipToExtraction`, `SetAlert` and `ARRConsole::ForceComplete` are `BlueprintCallable` in category `RunFlow` / `Console`, compiled in shipping. They need protected state, so they stay; move them to a `Cheats` category and a commented section, and early-return under `UE_BUILD_SHIPPING`. Category change has no Blueprint impact.
- H2 `URRStressSubsystem::DoesSupportWorldType` (RRStressSubsystem.cpp:179) is not shipping-gated, unlike `URRCheatSubsystem`.

## Test code readability
- H3 18 `F…Access` structs in 10 test files read protected members with the derived-struct member-pointer idiom (`Miner->*(&FMinerLoadoutAccess::StartingWeapons)`). Legal and consistent with the no-hook rule, but unexplained to a newcomer. Gather them in one `Tests/RRTestAccess.h` with a comment on the idiom.
- H4 repeated test lambdas (find pickups or projectiles by `TActorIterator`, in 6 files) belong in `RRTestHarness.h`.

## Dead or transitional code
- D1 `Variant_TwinStick/` (1,875 lines, 24 files): no Rogue Rigs class or asset references it; only `Content/Variant_TwinStick` Blueprints do. `RogueRigs.Build.cs` adds 5 `PublicIncludePaths` for it (against [[cpp-standard]]) and the StateTree modules. Deletion needs the bp agent to delete the content first: a separate task, not 020-02.
- D2 `URRGameplayAbility` (GAS/, 32 lines): no subclass, no user. Keep only if GAS abilities are planned for M2; else delete (no asset references it).
- D3 `ARRMiner::GuaranteedWeaponClasses` (native shotgun fallback): BP_Miner now lists BP_Shotgun in `StartingWeapons`, so in game it never adds anything; headless tests still rely on it.
- D4 `EnemyStats.csv` columns Damage, AttackRate, AttackRange, WindUp are 0 and unused for the kamikaze (see F2 in [[code-review-m1-gameplay]]).

## Files over ~800 lines
- `Tests/RRIntegrationTests.cpp` 2,059 (already 5 namespaces: split one file per namespace).
- `Grid/RRArenaGrid.cpp` 995 (F10).
- `Tests/RRWeaponTests.cpp` 985.
- Next in line: `RRDigTests.cpp` 662, `RRStressSubsystem.cpp` 634.

## Docs
- `sys-enemies` is about 1,100 words (limit about 400 per [[_authoring]]); it should be split when 020-02 touches it.

## Relations
- [[code-review-m1]] — parent index.
- [[sys-cheats]] — the cheat system these hooks serve.
- [[sys-automation-tests]] — the test harness H3 and H4 refer to.
