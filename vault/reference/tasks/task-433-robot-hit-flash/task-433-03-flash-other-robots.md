---
id: task-433-03-flash-other-robots
description: "Bp: Every other robot class flashes the same way, the kamikaze core excepted."
parent: task-433-robot-hit-flash
kind: task
status: in-progress
---
# Robot hit flash: bp

- goal: Every other robot class flashes the same way, the kamikaze core excepted.
- agent: bp
- depends_on: task-433-02-flash-chaser-material
- inputs: readability-rules (vault/reference/gdd/art-audio/readability-rules.md), color-code (vault/reference/gdd/art-audio/color-code.md), enemy-kamikaze (vault/reference/gdd/enemies/enemy-kamikaze.md), enemy-shooter (vault/reference/gdd/enemies/enemy-shooter.md), enemy-heavy (vault/reference/gdd/enemies/enemy-heavy.md)
- expected_output: see done_when
- done_when: Materials of shield chaser, kamikaze body, shooter and heavy (v1 and v2 where the game spawns them) compiled with no error and read the flash slot; the kamikaze core material is untouched; any class that cannot be done is listed with the reason.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: 2026-11-05T16:17:45Z
- waiting_for: none
- result: open

## Relations
- [[task-433-robot-hit-flash]] — parent global task.
