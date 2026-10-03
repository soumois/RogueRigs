---
id: sys-stress
description: "Stress test tooling as built: URRStressSubsystem, rr.Stress.Spawn/Clear/Peace/Run cheat commands, stat unit sampling, results CSV, launcher script."
parent: systems-index
kind: doc
---
# Stress tooling

- responsibility: put many robots in the arena on demand and measure frame time per robot count ([[stress-test-m1]]). Debug only, no gameplay role.
- main_classes: `URRStressSubsystem` (`Source/RogueRigs/Debug/RRStressSubsystem.h/.cpp`, `UTickableWorldSubsystem`, Game and PIE worlds); `FRRStressStepResult` (one step: count, frames, avg and p99 frame, game, draw, GPU ms). Launcher: `Scripts/Run-StressTest.ps1`.
- entry_points (console, all `ECVF_Cheat`):
  - `rr.Stress.Spawn <N> [Class]`: N robots on walkable cells reachable in the flow field ([[sys-flow-field]]), nearest first from `rr.Stress.MinCells` (3) BFS steps from the miner, one per free cell (wraps with a quarter-cell corner offset when cells run out), capsule `rr.Stress.FloorClearance` (2 uu) above the floor, ignoring `MaxAlive`. Default class: first `ARRChaser` subclass among the first spawner's classes (`ARREnemySpawner::GetSpawnableClasses`, [[sys-spawner]]); `Class` matches a spawnable class name (substring, case insensitive) or a class path.
  - `rr.Stress.Clear`: destroys the stress robots (no death, no XP).
  - `rr.Stress.Peace <0|1>`: loose `State.Invulnerable` on the miner ASC and `StopFiring` on every weapon (1), reverse (0), so counts hold.
  - `rr.Stress.Run [Counts] [Settle] [Measure] [Quit]`: `rr.Spawner.Enabled 0`, peace on, `stat unit` on; per count (separator `,` or `+`; default `rr.Stress.Counts` 50,100,200,300,400) tops the stress robots up, waits `Settle` s (`rr.Stress.SettleSeconds` 5), samples every frame for `Measure` s (`rr.Stress.MeasureSeconds` 20), logs one `Stress step:` line; at the end writes `Saved/Stress/Stress_<time>.csv` and exits when `Quit` is 1. CSV profiler events `Stress_<N>_Settle` / `_Measure` mark the steps.
  - C++: `SpawnStressEnemies`, `ClearStressEnemies`, `GetStressEnemyCount`, `SetPeace`, `StartRun`, `GetResults`, `ResolveEnemyClass`.
- data: none; tunables are the console variables above.
- depends_on: [[sys-flow-field]], [[sys-grid]], [[sys-spawner]], [[sys-enemies]], [[sys-player]], [[sys-weapons]], [[sys-gas-core]].
- implements: [[stress-test-m1]] method.
- gotchas: samples are the `FStatUnitData` raw values of the game viewport (frame = `FApp::GetDeltaTime`, game, render thread "Draw", GPU 0), refreshed only while `stat unit` is drawn; the overlay stays on during a run. The module does not link RenderCore/RHI, so the engine globals are not read directly. `-csvprofile` / `-csvCaptureFrames` wrote no file from the editor binary `-game` (task-013-01); use Insights for per-system costs. Robots converge on the miner: steady state is one dense crowd, the worst case for collision and avoidance. A fresh mass spawn and a step-by-step top-up give different numbers at the same count (±2 ms game thread).

## Relations
- [[systems-index]] — parent index.
- [[stress-test-m1]] — procedure and results this tool produces.
- [[sys-spawner]] — default robot class and the timed spawner the run disables.
- [[sys-cheats]] — cheat console variables (rr.Cheat.*) settable by MCP, unlike these commands.
