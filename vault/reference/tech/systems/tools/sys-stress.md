---
id: sys-stress
description: "Stress test tooling as built: URRStressSubsystem, rr.Stress.* commands, peace/fire/burst/chain/dig/walk/game run modes, spike tagging, grid subdivide and bench, results CSV, launcher."
parent: systems-tools-index
kind: doc
---
# Stress tooling

- responsibility: put many robots in the arena on demand and measure frame time per robot count ([[perf-stress-test-m1]]). Debug only, no gameplay role.
- main_classes: `URRStressSubsystem` (`Source/RogueRigs/Debug/RRStressSubsystem.h/.cpp`, `UTickableWorldSubsystem`, Game and PIE worlds, absent in shipping like `URRCheatSubsystem`); `FRRStressStepResult` (one step: count, frames, avg and p99 frame, game, draw, GPU ms; max frame and game; burst kills, barrels, explosions resolved; blocks destroyed, event frames, flow-field computations; robots alive avg / min / max, spikes and their causes, forced horde size, miner level); `ERRStressMode` (Peace, Fire, Burst, Chain, Dig, Walk, Game). Launcher: `Scripts/Run-StressTest.ps1 [-Counts] [-Settle] [-Measure] [-Mode] [-Class] [-PreExec "cmd,cmd"] [-Trace cpu,frame,bookmark,region] [-TraceFile] [-ExtraArgs] [-Exe]` `-Map` goes through `Repair-MsysGamePath`: a Git Bash-rewritten `<drive>:/...Git/[usr/]Game/<rest>` becomes `/Game/<rest>` and the script prints `map: repaired MSYS path -> /Game/<rest>` (task-402; same rule as capture.js, [[tool-capture]]) (Insights trace to `Saved/Traces/`, not committed; [[perf-trace-m1]]). `-Exe <staged>/RogueRigs/Binaries/Win64/RogueRigs.exe` runs the cooked Development game instead of the editor binary (no project path, no `-game`; CSV and log under `<staged>/RogueRigs/Saved/`); staging command in [[build-toolchain]]. Tests: `RogueRigs.Stress.*` ([[sys-automation-tests]]).
- entry_points (console, all `ECVF_Cheat`):
  - `rr.Stress.Spawn <N> [Class]`: N robots on walkable cells reachable in the flow field ([[sys-flow-field]]), nearest first from `rr.Stress.MinCells` (3) BFS steps from the miner, one per free cell (wraps with a quarter-cell corner offset when cells run out), capsule `rr.Stress.FloorClearance` (2 uu) above the floor, ignoring `MaxAlive`. Default class: first `ARRChaser` subclass among the first spawner's classes (`ARREnemySpawner::GetSpawnableClasses`, [[sys-spawner]]); `Class` matches a spawnable class name (substring, case insensitive) or a class path.
  - `rr.Stress.Clear`: destroys the stress robots (no death, no XP).
  - `rr.Stress.Pickups <Nuts> [Chips]` (task-138-01): Nuts of `rr.Stress.PickupClass` (`BP_PickupXP`) and Chips of `rr.Stress.PickupBigClass` (`BP_PickupXPBig`), chips spread among the nuts, through `URRPickupSubsystem::RequestSpawn` at cell centers (pooled path, no XP merge, [[sys-pickups-xp]]), same cell order as Spawn but only cells farther (2D) than the miner `PickupRadius` + largest `PopMaxDistance` + quarter-cell diagonal, so nothing is attracted; logs counts. `rr.Stress.ClearPickups`: releases them to the pool (no collection, no XP), logs the count alive before and magnetized.
  - `rr.Stress.Peace <0|1>`: loose `State.Invulnerable` on the miner ASC and `StopFiring` on every weapon (1), reverse (0), so counts hold.
  - C++: `SpawnStressEnemies`, `ClearStressEnemies`, `GetStressEnemyCount`, `SpawnStressPickups`, `ClearStressPickups`, `GetStressPickupCount`, `SetPeace`, `SetMinersInvulnerable`, `SetMinerWeaponsFiring`, `StartRun`, `GetResults`, `ResolveEnemyClass`; static `KillEnemy` (self-applied explosion-damage GE far above health: normal death, XP, kamikaze explosion), `DestroyAllBarrels`, `ParseMode`.
- data: none; tunables are the console variables above.
- depends_on: [[sys-flow-field]], [[sys-grid]], [[sys-spawner]], [[sys-enemies]], [[sys-player]], [[sys-weapons]], [[sys-gas-core]].
- implements: [[perf-stress-test-m1]] method.
- gotchas: samples are the `FStatUnitData` raw values of the game viewport (frame = `FApp::GetDeltaTime`, game, render thread "Draw", GPU 0), refreshed only while `stat unit` is drawn; the overlay stays on during a run. The module does not link RenderCore/RHI, so the engine globals are not read directly. `-csvprofile` / `-csvCaptureFrames` wrote no file from the editor binary `-game` (task-013-01); use Insights for per-system costs (`-Trace`; `-ExtraArgs -statnamedevents` adds engine stat scopes but ≈ +2 ms game thread at 400). Robots converge on the miner: steady state is one dense crowd, the worst case for collision and avoidance. A fresh mass spawn and a step-by-step top-up give different numbers at the same count (±2 ms game thread). Steps of 55 s measure + 5 s settle last ≈ 61 s, the GC interval: the full GC then lands in the top-up gap between measure windows and the max frame misses it; check GC frames in a trace or with `-ExtraArgs '-LogCmds="LogGarbage Log"'` (`GC purged` lines). Burst: barrels exist once per process, so run one count per launch; raise `rr.Stress.MinCells` (8) through `-PreExec` so kamikazes do not arm during the settle time. Pickups: delivery is synchronous (`SpawnMaxFrames` 0 during the call, restored); the miner must not move toward them (pacing bot off), a collected one is dropped from the list.

## Relations
- [[sys-stress-run-modes]] — rr.Stress.Run and its modes: peace, chain, grid granularity and real-game mode. Open to choose or add a stress mode.
- [[sys-stress-diagnostics]] — Stress diagnostics: spike logs, pool counters, C++ entry points, visual checks, behaviour while paused. Open to read stress output.
- [[systems-index]] — parent index.
- [[perf-stress-test-m1]] — procedure and results this tool produces.
- [[perf-stress-final-m1]] — final S0, S2, S7 results made with the run modes.
- [[sys-spawner]] — default robot class and the timed spawner the run disables.
- [[perf-trace-m1]] — trace captured with the launcher options and regions.
- [[perf-stress-grid-cells]] — 200 / 100 / 67 uu cell results made with the subdivide, bench, chain and dig tools; real-game game-mode results.
- [[sys-progression]] — pacing bot that drives the miner in game mode.
- [[perf-stress-damage-numbers]] — damage number check made with the visual check commands.
- [[sys-cheats]] — cheat console variables (rr.Cheat.*) settable by MCP, unlike these commands.
