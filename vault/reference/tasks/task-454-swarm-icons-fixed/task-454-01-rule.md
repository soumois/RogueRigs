---
id: task-454-01-rule
description: "Cpp: Kills stop moving the alert bar and the swarm marks until the first front is over."
parent: task-454-swarm-icons-fixed
kind: task
status: done
---
# First swarms ignore kills until the first front is over: cpp

- goal: Kills stop moving the alert bar and the swarm marks until the first front is over.
- agent: cpp
- depends_on: none
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), difficulty (vault/reference/gdd/run/difficulty.md), enemies-spawn-rules (vault/reference/gdd/enemies/enemies-spawn-rules.md), sys-hud-pressure (vault/reference/tech/systems/ui/sys-hud-pressure.md), sys-objectives (vault/reference/tech/systems/world/sys-objectives.md), sys-spawner-director (vault/reference/tech/systems/enemies/sys-spawner-director.md), sys-difficulty (vault/reference/tech/systems/core/sys-difficulty.md)
- expected_output: see done_when
- done_when: Build ok; new test as in A5 passes; Pressure, Objectives and Spawner test families pass, figures in the result.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-454
- usage: included in the executor run of task-454
- runs: 1
- milestone: M2
- started: 2026-11-05T23:32:49Z
- waiting_for: none
- result: Commit e04554fb. Kills still count (KillCount, threat) but add no alert until horde 0 of the run's spawner is Done (front finished spawning, Q30), FRRRunFlowConfig::bKillsWaitForFirstHorde (true), URRRunFlowSubsystem::DoKillsAddAlert(); no spawner, director off or no horde = old rule. Build ok, no new warning. New test RogueRigs.HUD.PressureKillsWaitFirstHorde (fill 20 s, fronts 2 s and 6 s): 18 kills before front 0 over at frame 181, alert 3.017 s = time alone, horde 0 mark 0.1000-0.1000, horde 1 mark 0.2992-0.3000; after, 4 kills add 2.000 s and horde 1 mark 0.2992 -> 0.4000. HUD.Pressure, Integration.Pressure, RunFlow, Console, Extraction, Spawner: 43/43 passed, no expectation changed.

## Relations
- [[task-454-swarm-icons-fixed]] — parent global task.
