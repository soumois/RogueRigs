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




- [[task-512-audio-shot-quieter]] — Lower the volume of the player shot sound, every weapon, as a table value the owner retouches by ear. Status in-progress.
- [[task-513-audio-wall-impact-distance]] — Split the wall impact from the enemy impact and make it quieter, fading with the distance from the player; values in the event table. Status todo.
- [[task-514-audio-enemy-death-distance]] — Enemy death sound fades with distance from the player, with the fade option built in task-513. Status wait-answers.
- [[task-515-audio-ore-pickup]] — Ore pickup plays the XP pickup sound for now, through its own event line so it can be swapped later. Status todo.
- [[task-516-audio-explosion-edge-distance]] — Placed explosion sound event, level fading with the distance from the explosion edge; import of the owner file. Status wait-answers.
- [[task-517-audio-dig-sound]] — Dig sound: one event at each dig hit of the miner on a block; import of the owner file. Status wait-answers.
