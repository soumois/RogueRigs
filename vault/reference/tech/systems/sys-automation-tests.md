---
id: sys-automation-tests
description: "C++ automation tests as built: RogueRigs.Dig.* (grid damage, drops, radius destruction, drone dig and cancel, pickup collection), test world harness, headless run, gotchas."
parent: systems-index
kind: doc
---
# Automation tests

- responsibility: prove gameplay rules headless, where MCP cannot drive PIE input. Currently the dig rules (task-006-05).
- main_classes: `Source/RogueRigs/Tests/RRDigTests.cpp` (all code inside `WITH_DEV_AUTOMATION_TESTS`; simple tests, flags `EditorContext | EngineFilter`). `URRTestEventRecorder` (`Tests/RRTestEventRecorder.h`): UObject with UFunctions bound to `ARRMiner::OnPickupCollected` and `ARRArenaGrid::OnCellDestroyed`; a UCLASS, so compiled in every configuration (UHT rejects the automation guard), never used by gameplay.
- harness: per test, a transient Game `UWorld` (no game mode: `AWorldSettings::NotifyBeginPlay` dispatches BeginPlay) with `BP_ArenaGrid` at the origin and, when needed, `BP_Miner` on (42, 38) (spawns `BP_Drone`), ticked manually at 1/60 s. Protected state is read through pointers to members formed in a never-instantiated derived struct.
- entry_points (tests):
  - `RogueRigs.Dig.RockDestroy`: `ApplyTerrainDamage(dt)` per frame on rock (43, 38) destroys it at frame 90 (1.5 s); rock HISM −1, floor HISM +1, every instance maps back to its cell (index, type, position), `OnCellDestroyed` once, no drop.
  - `RogueRigs.Dig.HealOreDrops`: heal ore (49, 45) drops at frames 90 and 180, is destroyed at 270 (4.5 s) with 3 drops, 5 `BP_PickupHeal` in total.
  - `RogueRigs.Dig.DestroyCellsInRadius`: every block within 2.5 cells of a point near (49, 45) destroyed (8), all others standing, destruction drops spawned, maps consistent.
  - `RogueRigs.Dig.DroneDig`: `RequestDig(grid, (43, 38), (−1, 0))` → Dig, block shaking; destroyed at frame 90; back to Orbit, no shaking cell, grid tick off, laser hidden.
  - `RogueRigs.Dig.DroneDigCancel`: after 0.5 s of dig, moving the miner 2 cells west cancels on the next tick; damage kept (health 1.0), none added afterwards.
  - `RogueRigs.Dig.PickupCollect`: a player controller (no local player) possesses the miner so player 0 lookup works; a pickup 100 uu away is collected (`OnPickupCollected` Heal, frame 23), one beyond `PickupRadius` is not.
- data: `arena_m1` layout, `PlayerBaseStats.csv`, the Blueprint classes above. Headless command in [[build-toolchain]].
- depends_on: [[sys-grid]], [[sys-drone]], [[sys-pickups]], [[sys-player]].
- implements: none (verification of [[ore-blocks]], [[heal-ore]], [[drone]]).
- gotchas: the tick task manager runs a tick function once per `GFrameCounter`; a synchronous test runs inside one engine frame, so the harness increments `GFrameCounter` before each `World->Tick` (without it actors tick once). Tests depend on `arena_m1` cells around (43, 38) and (49, 45).

## Relations
- [[systems-index]] — parent index.
- [[build-toolchain]] — the headless command that runs these tests.
- [[sys-grid]] — the terrain rules tested.
- [[sys-drone]] — the dig state machine tested.
- [[sys-pickups]] — the drop and collection rules tested.
