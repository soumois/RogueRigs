---
id: task-171-debris-throw-random
description: "Global task: debris throw speed times a random 1.3-2 factor per death, each piece turned within ±15° of the killing-hit direction."
parent: tasks-to-do-index
kind: task
status: todo
---
# Robot debris: random force and angle

- request: Ok pour la projection (plus haute et plus loin), il faudrait du random pour qu'un même robot ne projette pas toujours de la même force: plutôt entre 1.3 et 2. Rajoute un random d'angle de 15 degrés d'un côté ou de l'autre (toujours globalement dans la direction de ce qui les a tués, explosion comprise), appliqué à chaque pièce (owner, 2026-10-14)
- goal: Debris throw: speed times a random factor 1.3 to 2 drawn once per death, and each piece turned by a random angle within plus or minus 15 degrees around the killing-hit direction.
- acceptance: The system compiles with no stack issue and stays one-shot (finishes on its own); the changed module inputs read back; saved by explicit asset path; bp-robot-debris doc updated; committed by path; the editor stays open; committed.
- gdd_check: enemy-death read and updated with the owner values (2026-10-14): 2 to 4 pieces, throw factor 1.3-2 per death, ±15° per piece, start ±10 cm, 3 s then sink 0.4 s without shrink; colour and size left unchanged (owner did not ask).
- confirmed_by_owner: yes (2026-10-14)
- milestone: M3
- next_step: task-171-01-debris-throw-random
- result: open

## Subtasks
1. task-171-01-debris-throw-random — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-171-01-debris-throw-random]] — step 1, bp.
