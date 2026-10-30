---
id: task-389-01-clear-robot-effects
description: "Cpp: remove the robot death puffs and debris still playing when the next level is prepared"
parent: task-389-robot-particles-cleared-between-levels
kind: task
status: todo
---
# Robot particles removed between levels: cpp

- goal: Reproduce in -game (kill robots with rr.Debug.KillRobots or rr.Debug.SpawnNear then rr.Run.EndLevel in the same second, rr.Run.Stats.Next, rr.Run.Continue) and log the robot death effects still alive at level 2 start; then make the ClearingLevel step remove them (NS_RobotDeathPuff, NS_HeavyDeathPuff, NS_RobotDebris, NS_ShieldDebris), reusing what task-388 added when it fits. If task-388 already removes them, prove it by the log and change no code.
- agent: cpp
- depends_on: task-388-01-clear-chest-effects
- inputs: agent-cpp (vault/reference/agents/agent-cpp.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), bp-robot-death-puff (vault/reference/tech/blueprints/fx/bp-robot-death-puff.md), bp-heavy-death-puff (vault/reference/tech/blueprints/fx/bp-heavy-death-puff.md), bp-robot-debris (vault/reference/tech/blueprints/fx/bp-robot-debris.md), bp-shield-debris (vault/reference/tech/blueprints/fx/bp-shield-debris.md), sys-cheats (vault/reference/tech/systems/core/sys-cheats.md), task-388-01-clear-chest-effects (vault/reference/tasks/task-388-chest-glow-cleared-between-levels/task-388-01-clear-chest-effects.md) result and commit, global request and gdd_check: vault/reference/tasks/task-389-robot-particles-cleared-between-levels.md
- expected_output: C++ change (or none if task-388 covers it), log line with the count of robot effects removed, commit `task-389-01-clear-robot-effects: ...`; change summary with cause, files, log lines, for the doc subtask.
- done_when: Build passes with the editor closed; a -game log shows robot death effects spawned within 1 s before the extraction and, at the level 2 preparation, removed (count > 0), none alive at level 2 start; death effects still spawned during level 2; uncommitted RRPlayerController and Config changes untouched; committed by path.
- editor_state: closed
- token_ceiling: 1.2 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-389-robot-particles-cleared-between-levels]] — parent global task.
