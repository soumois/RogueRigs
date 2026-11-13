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




- [[task-516-audio-explosion-edge-distance]] — Placed explosion sound event, level fading with the distance from the explosion edge; import of the owner file. Status in-progress.
- [[task-517-audio-dig-sound]] — Dig sound: one event at each dig hit of the miner on a block; import of the owner file. Status todo.
- [[task-518-audio-level-up-not-heard]] — The level up sound is heard in its file but not in game: make it audible in game. Status todo.
- [[task-519-audio-music-three-tracks]] — Three looped music tracks for main menu, loading and in game, through the Music slider; in game quieter. Status todo.
