---
id: task-017-kamikaze-radius
description: "Global task: kamikaze explosion radius from 250 to 312 uu (owner +25 %), data, tests and docs; the explosion visual follows the radius."
parent: tasks-done-index
kind: task
status: done
---
# Kamikaze explosion radius +25 %

- request: "Augmenter le radius de l'explosion des kamikaze de 25%" [...] "oui lance les deux" (2026-10-04).
- goal: kamikaze explosions reach 312 uu for character damage and block destruction, and the grey-box sphere grows to the new radius.
- acceptance: (1) build exits 0; all tests pass, updated ones included; (2) the kamikaze stats data gives 312 uu and a test proves a block and the miner at about 300 uu are hit; (3) docs match; lint clean; committed.
- gdd_check: Read [[enemy-kamikaze]], [[explosions]]. GDD updated with the owner value before planning. The shotgun review note was added to [[shotgun]] (no work).
- confirmed_by_owner: yes (2026-10-04, all subtasks pre-approved)
- milestone: M1
- next_step: none
- result: All acceptance checks passed: radius 312 in data, FX follows, 98/98 tests, docs.

## Subtasks
1. task-017-01-radius — cpp: radius 312 in data, visual check, tests, docs

## Relations
- [[tasks-done-index]] — parent index.
- [[task-017-01-radius]] — step 1, cpp.
