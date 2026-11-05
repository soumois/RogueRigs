---
id: task-448-02-heavy-puff-check
description: "Bp: The heavy death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong."
parent: task-448-death-puffs-sim-check
kind: task
status: done
---
# Check the robot death puffs in the simulation: bp

- goal: The heavy death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong.
- agent: bp
- depends_on: task-448-01-robot-puff-check
- inputs: task-436-robot-death-particle (vault/reference/tasks/task-436-robot-death-particle.md), bp-heavy-death-puff (vault/reference/tech/blueprints/fx/bp-heavy-death-puff.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: see done_when
- done_when: Particle dump from a real heavy kill in PIE, per emitter, given in the result, matching the asset; asset unchanged unless the dump was wrong; no LogNiagara error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-448
- usage: included in the executor run of task-448
- runs: 1
- milestone: M2
- started: 2026-11-05T22:40:04Z
- waiting_for: none
- result: Done, no asset change: the compiled NS_HeavyDeathPuff plays the task-436 values. PIE L_ArenaM1, a BP_Heavy spawned with rr.Debug.SpawnNear and killed with rr.Debug.KillRobots 650 (KillEnemy, the normal death path) at slomo 0.05, 8 fx.Niagara.DumpComponents full dumps of the puff alive. Per emitter: Lines 6, burst t 0.030, life 0.141 to 0.184, size 21 x 238 to 356, colour (1.7, 0.8, 0.05), speed 952 to 2336; Core 1, t 0, life 0.32, size 840, colour (1, 1, 1); Blobs 6, t 0.060, life 0.223 to 0.277, size 125 to 145, colour (1.2, 0.3, 0.02), speed 510 to 1753; Puffs 5, t 0.170, life 0.205 to 0.245, size 143 to 166, colour (0.2, 0.022, 0.015), speed 83 to 225; z 256.15 for all (robot puff 124, so +130 against +60 as built). Alpha equals NormalizedAge on every row (error 0), 0.0007 to 0.994. No LogNiagara warning or error. Cheats, slomo and editor throttle restored, PIE stopped. Look in game not judged (no capture, A5).

## Relations
- [[task-448-death-puffs-sim-check]] — parent global task.
