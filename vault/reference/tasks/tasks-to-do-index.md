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
- [[task-388-chest-glow-cleared-between-levels]] — Nothing of an opened bonus chest (interior glow, opening rays and sparks, lid slam ring) stays visible in the next level of the run. Status wait-answers.
- [[task-389-robot-particles-cleared-between-levels]] — Death puffs and debris of robots killed on the previous level no longer show when the next level starts. Status wait-answers.
- [[task-390-run-won-back-to-main-menu]] — After the level 4 extraction, the run-won end screen button returns to the main menu instead of restarting a run. Status wait-answers.
- [[task-391-final-screens-count-level-ore]] — The stats screen Total Ore and the run-won Ore secured line include the ore secured by the extraction just made. Status wait-answers.
