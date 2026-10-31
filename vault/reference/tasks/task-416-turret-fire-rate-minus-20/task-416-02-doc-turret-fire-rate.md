---
id: task-416-02-doc-turret-fire-rate
description: "Doc: update sys-turret (TurretLauncher row FireRate 2.64)"
parent: task-416-turret-fire-rate-minus-20
kind: task
status: todo
---
# Turret fire rate lowered by 20 %: doc

- goal: Update the turret data values in the tech doc from the change summary of task-416-01: `TurretLauncher` row `FireRate` 2.64 (was 3.3, -20 %).
- agent: doc
- depends_on: task-416-01-turret-fire-rate-csv
- inputs: sys-turret (vault/reference/tech/systems/combat/sys-turret.md), task-416-turret-fire-rate-minus-20 (vault/reference/tasks/task-416-turret-fire-rate-minus-20.md) (global: request, gdd_check), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: sys-turret updated, lint clean, one commit `task-416-02-doc-turret-fire-rate: <summary>`.
- done_when: sys-turret `data:` line gives `FireRate` 2.64 with the task reference; no other node changed; lint clean; committed by path.
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
- [[task-416-turret-fire-rate-minus-20]] — parent global task.
