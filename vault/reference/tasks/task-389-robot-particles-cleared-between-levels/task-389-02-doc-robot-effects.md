---
id: task-389-02-doc-robot-effects
description: "Doc: update the preload and enemies docs with the robot effects cleared between levels"
parent: task-389-robot-particles-cleared-between-levels
kind: task
status: done
---
# Robot particles removed between levels: doc

- goal: Record what the ClearingLevel step now removes for robot death effects, the cause and the log verification.
- agent: doc
- depends_on: task-389-01-clear-robot-effects
- inputs: agent-doc (vault/reference/agents/agent-doc.md), sys-run-transition-preload (vault/reference/tech/systems/world/sys-run/sys-run-transition-preload.md), sys-enemies (vault/reference/tech/systems/enemies/sys-enemies/sys-enemies.md), global request and gdd_check: vault/reference/tasks/task-389-robot-particles-cleared-between-levels.md
- expected_output: sys-run-transition-preload (ClearingLevel list, log line), sys-enemies (gotcha on death effects and level change); commit `task-389-02-doc-robot-effects: ...`.
- done_when: The listed nodes state the clear and its log line; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: sonnet
- tokens: input 12, cache write 30733, cache read 105386, output 4920
- usage: turns 6, context start 12k peak 31k, resumes 0, 1 min, units 0.07 M
- runs: 1
- milestone: M2
- started: 2026-10-30T21:13:40Z
- waiting_for: none
- result: Documented the ClearingLevel robot effect clear and its log lines in sys-run-transition-preload, sys-enemies and bp-shield-debris; lint clean. Commit 4d6a8e18.

## Relations
- [[task-389-robot-particles-cleared-between-levels]] — parent global task.
