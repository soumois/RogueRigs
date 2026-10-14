---
id: tasks-to-do-index
description: "Open global tasks (in progress, todo, review, blocked), one line each with status. Read on every Orchestrator start."
parent: tasks-index
kind: note
---
# Tasks to do

A global task moves to [[tasks-done-index]] when it is done (`vault/scripts/task-status.js` does it).

## Relations
- [[tasks-index]] — parent index.
- [[task-168-upgrade-card-design]] — Redesign the stat upgrade cards: large cards with an image (stat icon, weapon icon plus stat icon when targeted), the effect, a pure lore line and a rarity-tinted frame, reroll below. Status in-progress.
- [[task-169-weapon-card-design]] — Give the new-weapon cards their own design: "New weapon" on top, the weapon icon only, the weapon name, its explanation, a lore line and a unique frame distinct from the rarity frames. Status in-progress.
- [[task-170-debris-sink-3s]] — Debris pieces stay 3 s at full size, then sink into the floor in 0.4 s without shrinking (3.4 s in all). Status todo.
- [[task-171-debris-throw-random]] — Debris throw: speed times a random factor 1.3 to 2 drawn once per death, and each piece turned by a random angle within plus or minus 15 degrees around the killing-hit direction. Status todo.
- [[task-172-debris-start-jitter]] — Debris pieces start at their place on the robot plus a random offset of up to 10 cm on each axis. Status todo.
- [[task-173-debris-count-variety]] — Throw 2 to 4 debris meshes per death with equal odds and check with a game run that the pieces drawn vary, fixing any bias. Status in-progress.
