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
- [[task-391-final-screens-count-level-ore]] — Every on-screen ore number (stats Total Ore, shop balance, run-won line, HUD) shows the ore the player holds, the balance. Status todo.
- [[task-393-miner-over-lit]] — The new Meshy miner looks burnt out (blown highlights) in the arena; reduce his brightness with the lever the owner picks. Status in-progress.
- [[task-396-duskite-leftover-labels]] — The last player-visible "ore" texts left by task-392 become "Duskite": shop card "Short on ore" and the end screen designer default. Status todo.
