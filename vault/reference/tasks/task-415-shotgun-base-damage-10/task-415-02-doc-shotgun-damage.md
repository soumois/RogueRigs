---
id: task-415-02-doc-shotgun-damage
description: "Doc: update sys-weapons (Shotgun row damage 10)"
parent: task-415-shotgun-base-damage-10
kind: task
status: todo
---
# Shotgun base damage 10: doc

- goal: Update the shotgun data values in the tech doc from the change summary of task-415-01: `Shotgun` row damage 10 per pellet (was 6).
- agent: doc
- depends_on: task-415-01-shotgun-damage-csv
- inputs: sys-weapons (vault/reference/tech/systems/combat/sys-weapons.md), task-415-shotgun-base-damage-10 (vault/reference/tasks/task-415-shotgun-base-damage-10.md) (global: request, gdd_check), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: sys-weapons updated, lint clean, one commit `task-415-02-doc-shotgun-damage: <summary>`.
- done_when: sys-weapons `data:` line gives the Shotgun damage as 10 per pellet with the task reference; no other node changed; lint clean; committed by path.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-415-shotgun-base-damage-10]] — parent global task.
