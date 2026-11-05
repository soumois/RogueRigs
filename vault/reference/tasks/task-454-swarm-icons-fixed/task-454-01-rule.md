---
id: task-454-01-rule
description: "Cpp: Kills stop moving the alert bar and the swarm marks until the first front is over."
parent: task-454-swarm-icons-fixed
kind: task
status: in-progress
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T23:32:49Z
- waiting_for: none
- result: open

## Relations
- [[task-454-swarm-icons-fixed]] — parent global task.
