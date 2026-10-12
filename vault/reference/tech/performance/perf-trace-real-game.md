---
id: perf-trace-real-game
description: "Insights traces of the real-game run at 250 robots: game-thread cost ranking, spawn and XP burst spikes, GC, HUD cost before/after the refactor."
parent: performance-index
kind: doc
---
# Insights trace, packaged real game at 250 robots (task-068-01, 2026-10-10)

Staged Development build ([[build-toolchain]]), HEAD 0525a68, `Run-StressTest.ps1 -Exe … -Counts 250 -Settle 30 -Measure 60 -Mode game -PreExec "rr.Pacing.RunLookahead 0" -Trace cpu,frame,gpu,bookmark,region -ExtraArgs -statnamedevents` ([[sys-stress]]). Named events add ≈ 1.7 ms of game thread (game avg 12.11 traced vs 10.42 plain). Window: region `Stress_250_Measure`, 4826 frames, 204 robots avg. Analysis as in [[perf-trace-m1]]; the trace path must contain no space either.

## Game thread, ms per frame (inclusive)
| Scope | ms | per frame |
|---|---|---|
| `FEngineLoop::Tick` | 12.47 | 1 |
| `UWorld_Tick` / `TG_PrePhysics` | 9.91 / 7.17 | |
| Tick functions (all) | 6.80 | 656 |
| CharacterMovement `TickComponent` | 4.91 | 207 |
| … `EndScopedMovementUpdate` (child transforms 1.00, overlaps 0.86) | 2.12 | 209 |
| … `Char PhysWalking` / move sweeps (`MoveComponent`) | 1.89 / 1.68 | 205 / 391 |
| … scene queries total | 1.67 | 603 |
| `TG_EndPhysics` (physics wait) | 1.31 | 1 |
| `FViewport_Draw` (game-thread side) | 0.97 | 1 |
| Slate tick + paint (`WBP_HUD` Blueprint tick 0.40) | 0.86 | 1 |
| Tickable objects (stress 0.22, pickups 0.19, pacing bot 0.15) | 0.65 | |
| Robot actor ticks (`BP_Chaser_C` 0.40, `BP_Kamikaze_C` 0.14) | 0.58 | 190 |
| Niagara (manager 0.25, finalize 0.14, components 0.06) | 0.45 | |
| End-of-frame render updates | 0.39 | 1 |
| `SpawnActor` (waves and XP cubes, averaged) | 0.21 | 0.9 |
| `BP_EnemySpawner` tick (director waves, max 14.1) | 0.15 | 1 |
| GAS effects, flow field, explosions, timers | < 0.03 each | |

## Spikes (every frame > 22 ms of the window, 65 frames)
- Robot wave, 6–17 robots in one frame (harness fill 52, director 4): 0.74 ms per robot in `SpawnActor` (17 robots = 12.5 ms): `PostActorConstruction` 0.30 (BeginPlay 0.21, almost all in the unscoped `ARREnemy`/BP BeginPlay), object construction 0.17, component registration 0.12–0.16, Blueprint component instancing 0.11 (2 per robot), overlap and floor queries ≈ 0.05; plus ≈ 2.7 ms of fill bookkeeping. Frames 22–30 ms.
- XP burst, 24–195 cubes in one frame (9 frames): robots killed together finish their death shrink on the same frame, and `ARREnemy::UpdateDeath` → `DropXP` spawns all cubes and destroys the robots inside their actor ticks. 43.03 s: 67 robots destroyed (4.3 ms), 195 `BP_PickupXP_C` spawned (`SpawnActor` 9.0 ms + component instancing incl. the `TrailFX` Niagara component 5.1 ms), registration and unregistration ≈ 5 ms: 34.9 ms frame, ≈ 0.37 ms per dying robot with 3 cubes. This is the untagged spike of [[stress-grid-cells]] (robot count dropping).
- GC: one full GC at 66.2 s, 5.3 ms (reachability 1.0 ms) in a 16.9 ms frame, then 20 frames of incremental purge at 2.1 ms: no spike. Editor `-game` GC frame 49 ms (editor-only cost, [[perf-trace-m1]]).
- No PSO, shader compile, streaming or async loading event in any spike frame.

## Next optimisation targets, ranked
1. CharacterMovement, 4.9 ms per frame at 207 robots (movement updates and sweeps).
2. Robot spawning, 0.74 ms per robot: pool robots or spread a wave over frames.
3. XP drop: spawning 3 actors per death, 0.37 ms per death: pool cubes or spread the drops.
4. HUD Blueprint tick 0.40 ms (gone since task-105-01, below) and robot BP actor ticks 0.58 ms.

## After the HUD refactor (task-105-01, 2026-10-11)
Editor `-game` (not packaged), same command, channels and `-statnamedevents`; 4297 frames, 201 robots avg; run numbers in [[stress-grid-cells]]. Blueprint function scopes are visible by name.

| Game thread, ms per frame | 068-01 | 105-01 |
|---|---|---|
| `FEngineLoop::Tick` | 12.47 | 14.00 |
| Slate (`Total Slate Tick Time`) | 0.86 | 0.69 |
| … HUD Blueprint | 0.40 (`WBP_HUD` tick, ≈ 250 nodes) | 0.11 (panel events; `WBP_HUD` tick scope 0.005) |
| … HUD paint (`WBP_HUD` with panels) | not split | 0.13 |
| `FViewport_Draw` | 0.97 | 1.75 (`stat unit` overlay 0.26) |
| Tickable objects | 0.65 | 1.20 (pacing bot 0.37, pickups 0.29, stress 0.23) |

Panels, Blueprint ms per frame (event entries per frame) / paint: `WBP_MinimapLocator` 0.079 (0.97: locator view 0.64, minimap view 0.34, arrow pulse 1.51 calls) / 0.012; `WBP_XPStatus` 0.015 (0.49, one per XP cube) / 0.007; `WBP_Weapons` 0.009 (0.08) / 0.034; `WBP_AlertObjectives` 0.002 (0.03) / 0.023; `WBP_UpgradeChoice` 22 calls, `WBP_Subtitles` 3 calls: below resolution; `WBP_HealthStatus` no Blueprint scope / 0.025. In total ≈ 1.6 event entries and ≈ 6.4 named Blueprint function scopes per frame. The 068-01 trace showed one tick scope per frame, so the two counts are not the same unit.

Top game-thread costs after: tick functions 6.76 (1005 per frame); robot movement 3.46 (203: `EndScopedMovementUpdate` 1.89, `MoveComponent` 1.74); `FViewport_Draw` 1.75; tickables 1.20; end-of-frame render updates 1.13; `TG_EndPhysics` 1.01; Slate 0.69; robot BP actor ticks 0.56 (`BP_Chaser_C` 0.33); Niagara manager 0.39; ability system components 0.24; `BP_EnemySpawner` 0.20.

Comparable: the HUD rows (both scoped by name, close robot counts). The HUD Blueprint drops 0.40 → 0.11 ms, and the locator and minimap view events of a constantly moving miner make up most of the rest. Not comparable: the whole game thread, viewport draw and tickables. 068-01 was a packaged build (≈ 1.5–3.5 ms less game thread, no editor ticks or overlay), and the PC load differs by ≈ 2 ms from run to run.

## Relations
- [[performance-index]] — parent index.
- [[stress-grid-cells]] — packaged vs editor tables and the spike lines this trace explains.
- [[perf-trace-m1]] — the S0/S2 trace of the same method and the editor GC hitch.
- [[sys-stress]] — game mode, launcher and spike tags.
- [[task-068-01-shipping-insights]] — the task that produced this trace.
- [[task-105-01-hud-insights]] — the task that produced the after-refactor HUD trace.
- [[sys-hud]] — the refactored HUD whose panels are measured.
