---
id: task-448-01-robot-puff-check
description: "Bp: The robot death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong."
parent: task-448-death-puffs-sim-check
kind: task
status: done
---
# Check the robot death puffs in the simulation: bp

- goal: The robot death puff is read in the simulation on a real kill, and fixed only if its compiled values are wrong.
- agent: bp
- depends_on: none
- inputs: task-436-robot-death-particle (vault/reference/tasks/task-436-robot-death-particle.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), unreal-mcp-quirks (vault/reference/tech/pipelines/unreal-mcp-quirks.md)
- expected_output: see done_when
- done_when: Particle dump from a real kill in PIE, per emitter, given in the result, matching the asset; asset unchanged unless the dump was wrong; no LogNiagara error.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-448
- usage: included in the executor run of task-448
- runs: 1
- milestone: M2
- started: 2026-11-05T22:33:16Z
- waiting_for: none
- result: Done, no asset change: the compiled NS_RobotDeathPuff plays the task-436 values. PIE L_ArenaM1, real kills through rr.Debug.KillRobots (KillEnemy, the normal death path) at slomo 0.05, 8 fx.Niagara.DumpComponents full dumps, 5 puffs read alive. Per emitter (all 5 puffs): Lines 6 per puff, burst t 0.030, life 0.144 to 0.199, size 7 x 72 to 117, colour (1.7, 0.8, 0.05), speed 326 to 790; Core 1, t 0, life 0.32, size 280, colour (1, 1, 1); Blobs 6, t 0.060, life 0.221 to 0.279, size 34 to 49, colour (1.2, 0.3, 0.02), speed 166 to 577; Puffs 5, t 0.170, life 0.201 to 0.249, size 43 to 66, colour (0.2, 0.022, 0.015), speed 28 to 84; z 124 for all. Alpha equals NormalizedAge on every row (error 0), 0.006 to 0.9995, never stuck at 0. No LogNiagara warning or error. Probe traps: an open level-up screen pauses the world, so pooled puffs stay Active with 0 particles (pick the cards first); the background editor throttles PIE to about 3 fps (EditorPerformanceSettings bThrottleCPUWhenNotForeground set false for the check, restored after). Look in game not judged (no capture, A5).

## Relations
- [[task-448-death-puffs-sim-check]] — parent global task.
