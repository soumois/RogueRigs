---
id: task-525-01-single-marker
description: "Cpp: one horde "!" at a time per Q11"
parent: task-525-horde-marker-one-at-a-time
kind: task
status: done
---
# Un seul point d'exclamation de horde à la fois: cpp

- goal: one horde "!" at a time per Q11
- agent: cpp
- depends_on: none
- inputs: level-flow (vault/reference/gdd/run/level-flow.md), enemies-spawn-rules (vault/reference/gdd/enemies/enemies-spawn-rules.md), task-454-swarm-icons-fixed (vault/reference/tasks/task-454-swarm-icons-fixed.md), task-460-alert-letters-readable (vault/reference/tasks/task-460-alert-letters-readable.md)
- expected_output: see done_when
- done_when: A1 to A5 (code part) hold; commit.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-525
- usage: included in the executor run of task-525
- runs: 1
- milestone: M2
- started: 2026-11-13T13:59:18Z
- waiting_for: none
- result: commit 2d56f345: URRRunFlowSubsystem::ComputeAlertBarEvents lists one horde, the first whose state is not Done (front over, same moment kills start adding alert), none after the last; positions unchanged (horde 0 fixed 0.483 after the console, task-524). No Blueprint change: WBP_AlertObjectives ShowAlertIcons collapses all and places hordes by Index. Build green; RogueRigs.HUD + RogueRigs.Integration.Pressure 18/18 pass; expectations updated in HUD.PressureHorde (no mark after the last front, +2 calls), HUD.PressureKillsWaitFirstHorde (horde 1 hidden until front 0 over, then at 0.3), Integration.PressureHordeBanner (4 events, horde 1 collapsed then shown).

## Relations
- [[task-525-horde-marker-one-at-a-time]] — parent global task.
