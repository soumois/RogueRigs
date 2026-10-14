---
id: task-170-debris-sink-3s
description: "Global task: Debris pieces stay 3 s at full size, then sink into the floor in 0.4 s without shrinking (3.4 s in all)."
parent: tasks-done-index
kind: task
status: done
---
# Robot debris: 3 s, then sink in 0.4 s

- request: Passe à 3 sec et enfonce-les dans le sol en 0.4 sec après les 3 sec; garde l'enfoncement, pas le rétrécissement (owner, 2026-10-14)
- goal: Debris pieces stay 3 s at full size, then sink into the floor in 0.4 s without shrinking (3.4 s in all).
- acceptance: The system compiles with no stack issue and stays one-shot (finishes on its own); the changed module inputs read back; saved by explicit asset path; bp-robot-debris doc updated; committed by path; the editor stays open; committed.
- gdd_check: enemy-death read and updated with the owner values (2026-10-14): 2 to 4 pieces, throw factor 1.3-2 per death, ±15° per piece, start ±10 cm, 3 s then sink 0.4 s without shrink; colour and size left unchanged (owner did not ask).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: none
- result: Done. NS_RobotDebris: lifetime 3.4 s, ScaleMeshSize removed, collision plane dropped 10 m from age 3.0 s, post-solver velocity (0,0,-140) from 3.0 s (about 62-66 cm sink, computed); compiles clean; motion not seen; bp-robot-debris updated (2 to 4 pieces); commit f59f4bc

## Subtasks
1. task-170-01-debris-sink-3s — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-170-01-debris-sink-3s]] — step 1, bp.
