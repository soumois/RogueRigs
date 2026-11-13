---
id: task-541-03-doc
description: "Doc: Update the HUD doc node for the removed text"
parent: task-541-hud-turret-count-removed
kind: task
status: done
---
# HUD: remove the "Turrets: 2 / 2" text above the health bar: doc

- goal: Update the HUD doc node for the removed text
- agent: doc
- depends_on: task-541-02-capture
- inputs: 
- expected_output: see done_when
- done_when: The HUD bp/sys node no longer lists the turret count text; lint clean.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-541
- usage: included in the executor run of task-541
- runs: 1
- milestone: M2
- started: 2026-11-13T23:13:54Z
- waiting_for: none
- result: wbp-weapons updated (commit d79aa4c0): description, purpose, components, variables, ShowSlot and ProgressSlot no longer list the turret line; task-452-03 and task-461 entries marked removed; new task-541-01 entry. Lint clean. Run over the light budget (ALERT since subtask 1, about 48 turns, under 0.6 M units): cause in task-541-01/02 results.

## Relations
- [[task-541-hud-turret-count-removed]] — parent global task.
