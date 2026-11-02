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
- [[task-415-aim-laser]] — Replace the white aim chevrons of the miner by a thin white-gold aim laser from the rifle muzzle, as in the owner reference video. Status in-progress.
- [[task-416-camera-shake-mixing-falloff]] — Shared camera shake: shakes of different sources add up with a cap, and a shake at a world point fades with distance up to a max. Status in-progress.
- [[task-417-camera-shake-on-damage]] — The camera shakes lightly each time the miner loses health, mixed with any explosion or slam shake playing. Status todo.
- [[task-418-camera-shake-on-explosions]] — Each barrel or kamikaze explosion shakes the camera, a bit stronger when closer, nothing beyond a max distance. Status todo.
- [[task-419-camera-shake-on-heavy-slam]] — The heavy ground slam shakes the camera when the miner is within a max distance, a bit stronger when closer. Status todo.
