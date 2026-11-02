---
id: task-415-05-doc-aim-laser
description: "Doc: rewrite sys-aim-indicator for the laser; update sys-player"
parent: task-415-aim-laser
kind: task
status: in-progress
---
# Aim laser replaces the white aim chevrons: doc

- goal: Document task-415: rewrite sys-aim-indicator for the aim laser (component, Plane mesh, M_AimLaser parameter contract and defaults, Muzzle origin and height, stop at the first robot or rock, hiding, where to tune, chevron material removed) and update sys-player (the miner aim indicator component); facts only from the commits and change summaries; decided values from the global request, gdd_check and Questions.
- agent: doc
- depends_on: task-415-01-mat-aim-laser, task-415-02-aim-laser-component, task-415-03-delete-aim-chevrons
- inputs: task-415-aim-laser (vault/reference/tasks/task-415-aim-laser.md), sys-aim-indicator (vault/reference/tech/systems/combat/sys-aim-indicator.md), sys-player (vault/reference/tech/systems/core/sys-player.md), aim-indicator (vault/reference/gdd/art-audio/aim-indicator.md), agent-doc (vault/reference/agents/agent-doc.md)
- expected_output: Updated vault/reference/tech/systems/combat/sys-aim-indicator.md and sys-player; lint clean; one commit by path.
- done_when: sys-aim-indicator describes the laser and no longer the chevrons, with the task-415 commit hashes; sys-player updated; `node vault/scripts/lint.js --write` clean; committed by path as `task-415-05-doc-aim-laser: <summary>`.
- editor_state: any
- token_ceiling: 0.3 M units
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M3
- started: 2026-11-02T18:03:43Z
- waiting_for: none
- result: open

## Relations
- [[task-415-aim-laser]] — parent global task.
