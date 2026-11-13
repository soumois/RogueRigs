---
id: task-541-03-doc
description: "Doc: Update the HUD doc node for the removed text"
parent: task-541-hud-turret-count-removed
kind: task
status: todo
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
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-541-hud-turret-count-removed]] — parent global task.
