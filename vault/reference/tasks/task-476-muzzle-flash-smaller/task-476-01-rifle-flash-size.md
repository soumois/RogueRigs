---
id: task-476-01-rifle-flash-size
description: "Bp: The burst rifle green muzzle flash is shown about 30 % smaller, look unchanged; turret and shotgun flashes keep their size."
parent: task-476-muzzle-flash-smaller
kind: task
status: in-progress
---
# Player muzzle flash a little smaller: bp

- goal: The burst rifle green muzzle flash is shown about 30 % smaller, look unchanged; turret and shotgun flashes keep their size.
- agent: bp
- depends_on: none
- inputs: burst-rifle (vault/reference/gdd/weapons/burst-rifle.md), task-464-player-shot-colour (vault/reference/tasks/task-464-player-shot-colour.md), Art/Source/muzzle-flash/owner-ref-green-laser-muzzle-2026-11-05.webp
- expected_output: see done_when
- done_when: A1 to A5 hold in the editor and a -game frame; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-06T19:26:30Z
- waiting_for: none
- result: open

## Relations
- [[task-476-muzzle-flash-smaller]] — parent global task.
