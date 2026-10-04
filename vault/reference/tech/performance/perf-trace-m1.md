---
id: perf-trace-m1
description: "Insights CPU trace at 400 robots (S0, S2): game-thread ms per timer, CharacterMovement breakdown, 61 s hitch identified as editor-process GC; how to capture."
parent: performance-index
kind: doc
---
# Insights trace at 400 robots (task-018-01, 2026-10-04)

Same PC and `-game` editor binary as [[stress-final-m1]]. Capture: `Scripts/Run-StressTest.ps1 -Trace cpu,frame,bookmark,region [-ExtraArgs -statnamedevents]` ([[sys-stress]]); window = region `Stress_400_Measure` (20 s). Analysis headless: `UnrealInsights.exe -OpenTraceFile=<utrace> -AutoQuit -NoUI -ExecOnAnalysisCompleteCmd="@=<rsp>"`, rsp lines `TimingInsights.ExportTimerStatistics <csv> -threads=GameThread -region=Stress_400_Measure` and `TimingInsights.ExportTimingEvents ...`. CSV paths must not contain spaces.

## Game thread, ms per frame (plain trace, stepped run 50→400)
| Timer (inclusive) | S0 | S2 |
|---|---|---|
| Game thread (`Tick_Engine`) | 11.09 | 13.20 |
| `UWorld_Tick` | 10.01 | 11.58 |
| Component ticks (1211 per frame) | 6.78 | 8.04 |
| CharacterMovement `TickComponent` (400) | 5.89 | 6.97 |
| `FViewport_Draw` (game-thread side) | 0.88 | 1.38 |
| Skeletal mesh ticks (`CharacterMesh0`) | 0.61 | 0.74 |
| Tick-task wait (`WaitForTasks`) | 0.52 | 0.64 |
| End-of-frame render updates | 0.41 | 0.90 |
| Slate (tick + draw, HUD paint 0.03) | ≈ 0.50 | ≈ 0.35 |
| `URREnemyMovementComponent` (flow-field steering) | 0.26 | 0.28 |
| Fire only: damage text, projectiles, `AutoFire` | — | ≈ 0.45 |

Negligible: GAS (no ability system tick scope), `URRFlowFieldSubsystem` < 0.01, `URRRunFlowSubsystem` 0.05, `stat unit` overlay 0.12. S2 − S0: +1.08 ms CharacterMovement (likely knockback), +0.5 ms render updates, +0.45 ms fire actors.

## CharacterMovement breakdown (S0 400, `-statnamedevents`, total 7.71 ms with marker overhead)
| Scope | ms | calls/frame |
|---|---|---|
| `Char PhysWalking` | 4.65 | 400 |
| `MoveComponent(Primitive)` (move sweeps `GeomSweepMultiple` 1.48) | 2.15 | 753 (849 sweeps) |
| `Char FindFloor` (`ComputeFloorDist`, `GeomSweepSingle`) | 1.86 | 383 |
| `EndScopedMovementUpdate` (attached mesh transforms 0.81, body transforms 0.39, overlaps 0.19) | 1.64 | 419 |
| `Avoidance Time` (RVO) | 0.33 | 800 |
| Blocking hits (`DispatchBlockingHit` / `HandleImpact`) | 0.05 / 0.04 | 61 / 27 |
S2 same shape (+0.4 ms). Scene queries total 2.9 ms.

## 61 s hitch
Frames at trace 76.4 s (200 robots) and 137.5 s (400), 61.1 s apart; S2 at 136.3 s. Each 40–44 ms frame = normal tick (≈ 9–11 ms) + `ConditionalCollectGarbage` 29.5–32.6 ms: `PyUtil::CollectGarbage` 9.1–11.4 ms (Python editor plugin, pre-GC broadcast), reachability analysis 18.2–19.3 ms (`PerformReachabilityAnalysisOnObjectsInternal` ≈ 17 ms), gather/purge ≈ 1.5 ms. Cause: full GC at the default interval, in an editor process (Python plugin and editor objects in the object graph). Robots add little: the 200 and 400 hitches cost the same. A packaged build must be checked before any GC change. One 78 ms frame at the 200→300 step is the top-up spawn, not sampled.

## Relations
- [[performance-index]] — parent index.
- [[perf-risks]] — leads re-ranked from these numbers.
- [[stress-final-m1]] — the stat unit tables this trace explains.
- [[sys-stress]] — launcher trace options and the measure regions.
