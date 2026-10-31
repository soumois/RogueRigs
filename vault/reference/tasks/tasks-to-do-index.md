---
id: tasks-to-do-index
description: "Open global tasks (in progress, wait-answers, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done or cancelled (`vault/scripts/task-status.js` does it). A global waiting for the owner stays here as `Status wait-answers.`; its node's `waiting_for` says what it waits for.

## Relations
- [[tasks-index]] — parent index.
- [[task-314-chest-lid-logo]] — Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game. Status todo.
- [[task-413-bullet-shown-at-muzzle]] — the burst rifle bullet visually starts ahead of the Muzzle; measure the cause in game and make its first shown frame sit at the Muzzle. Status in-progress.
- [[task-415-shotgun-base-damage-10]] — raise the shotgun base damage from 6 to 10 per pellet in WeaponStats.csv, tests and docs. Status wait-answers.
- [[task-416-turret-fire-rate-minus-20]] — lower the turret fire rate by 20 %, 3.3 to 2.64 shots/s, in WeaponStats.csv, tests and docs. Status wait-answers.
