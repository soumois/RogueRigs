---
id: task-172-debris-start-jitter
description: "Global task: Debris pieces start at their place on the robot plus a random offset of up to 10 cm on each axis."
parent: tasks-done-index
kind: task
status: done
---
# Robot debris: random start position

- request: Il faut aussi un random dans la position de départ des pièces (owner, 2026-10-14)
- goal: Debris pieces start at their place on the robot plus a random offset of up to 10 cm on each axis.
- acceptance: The system compiles with no stack issue and stays one-shot (finishes on its own); the changed module inputs read back; saved by explicit asset path; bp-robot-debris doc updated; committed by path; the editor stays open; committed.
- gdd_check: enemy-death read and updated with the owner values (2026-10-14): 2 to 4 pieces, throw factor 1.3-2 per death, ±15° per piece, start ±10 cm, 3 s then sink 0.4 s without shrink; colour and size left unchanged (owner did not ask).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: none
- result: Done. Position spawn module: rotated baked offset + per-axis rand ±10 cm, Z clamped at system Z; module order and loop settings (Once, Complete) read back for 170-172; system bDeterminism set false (was true, seed 0); compiles clean; not run in PIE; commits 144c7a3, 86a489b

## Subtasks
1. task-172-01-debris-start-jitter — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-172-01-debris-start-jitter]] — step 1, bp.
