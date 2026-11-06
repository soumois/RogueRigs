---
id: task-464-02-turret-laser-green
description: "Bp: Turret shots and their impact sparks in the same green look as the rifle laser"
parent: task-464-player-shot-colour
kind: task
status: todo
---
# Player shot colour: bp

- goal: Turret shots and their impact sparks in the same green look as the rifle laser
- agent: bp
- depends_on: task-464-01-rifle-laser-green
- inputs: color-code (vault/reference/gdd/art-audio/color-code.md), turret (vault/reference/gdd/weapons/turret.md), task-464-01-rifle-laser-green (vault/reference/tasks/task-464-player-shot-colour/task-464-01-rifle-laser-green.md)
- expected_output: see done_when
- done_when: In a -game run turret shots and their impact sparks match the rifle laser look of task-464-01; turrets still have no muzzle flash; turret behaviour and values unchanged.
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
- [[task-464-player-shot-colour]] — parent global task.
