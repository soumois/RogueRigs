---
id: task-464-04-turret-muzzle-flash
description: "Bp: Turrets show the green muzzle flash of task-464-03 at their barrel on each shot, sized to the turret"
parent: task-464-player-shot-colour
kind: task
status: in-progress
---
# Player shot colour: bp

- goal: Turrets show the green muzzle flash of task-464-03 at their barrel on each shot, sized to the turret
- agent: bp
- depends_on: task-464-03-muzzle-flash-green
- inputs: turret (vault/reference/gdd/weapons/turret.md), burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-464-03-muzzle-flash-green (vault/reference/tasks/task-464-player-shot-colour/task-464-03-muzzle-flash-green.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: In a -game run each turret shot shows the green muzzle flash at the end of the turret barrel, sized to the turret, same look as the rifle flash; turret behaviour and values unchanged; robots still have no flash.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T09:13:51Z
- waiting_for: none
- result: open

## Relations
- [[task-464-player-shot-colour]] — parent global task.
