---
id: sys-stress-diagnostics
description: "Stress diagnostics: spike logs, pool counters, C++ entry points, visual checks, behaviour while paused. Open to read stress output."
parent: sys-stress
kind: doc
---
# Stress — diagnostics

  - Spikes (every mode): a measured frame over `rr.Stress.SpikeMs` (25) logs `Stress spike:` (t into the measure, frame, game, draw, GPU, last flow ms, robots, blocks, explosions) tagged `fill robots` (game-mode wave of the previous frame; since task-091-01 also a frame whose last two samples saw the actor pool create robots, logged `pool created`, robot creations only since task-092-01), `xp drop` (task-092-01: XP cubes requested on the last two samples, logged with their count; counted in the `Stress pickup pool:` line, not a CSV column), `gc` (post garbage collect delegate) or `after pause` (level-up screen); CSV columns `AliveAvg,AliveMin,AliveMax,Spikes,SpikesFill,SpikesGC,SpikesPause,Horde,Level`.
  - Pool (task-091-01): each step resets the pool counters and logs `Stress pool:` (created, reused, longest wait in frames, pool size, idle, acquired, pending; all classes) and, since task-092-01, `Stress pickup pool:` (same counters over the pickup classes, plus the `xp drop` spike count); the game-mode fill counts alive + pending robots against the cap.
  - Niagara pools (task-181-01): the measure window start snapshots the played and skipped counters of every `URRNiagaraPoolSubsystem` pool ([[sys-actor-pool]]; before the burst of S7); the `Stress step:` line ends with `niagara pools, <System> plays N skips N` per pooled system (sorted by name, `none` without pools) and the CSV gets one `<System>Plays,<System>Skips` column pair per system seen in any step (e.g. `NS_RobotDeathPuffPlays`, `NS_RobotDebrisSkips`). Plays = components activated, skips = deaths whose effect was refused because the pool was full.
  - Visual checks (`Debug/RRVisualCheckCommands.cpp`, `ECVF_Cheat`, compiled out of shipping): `rr.Debug.Shots <Delay> <Count> <Interval> [Prefix]` takes game viewport screenshots (no UI) to `Saved/Screenshots/<platform>/<Prefix>_NN.png` on a real-time ticker and logs the damage number state (pushed, pending, component, live and spawned particles) per shot; `rr.Debug.CritChance <0..1>` sets the CritChance base of every weapon. Use through `-PreExec` with `slomo 0.25` to catch short-lived effects ([[perf-stress-damage-numbers]]). A screenshot frame costs ≈ 0.5 s.
  - While a run is paused (level-up screen from burst XP) the subsystem still ticks, picks card 0 (`RRCheats::ChooseUpgrade`) and skips sampling.

## Relations
- [[sys-stress]] — parent hub: summary and verdict.
- [[perf-stress-damage-numbers]] — referenced above.
- [[sys-actor-pool]] — the Niagara pool whose counters are logged.
- [[perf-stress-chaser-meshy]] — first results with the puff and debris counters.
