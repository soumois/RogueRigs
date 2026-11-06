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


- [[task-471-wall-look-reference]] — Terrain walls get the look of owner image 2: light-lilac tops, a glowing warm orange rim along the top edge, dark stratified sides; checked by capture against the image. Status todo.
- [[task-472-actor-blob-shadows]] — Each listed actor gets a soft purple-grey shadow directly beneath it that follows it and ignores every light, with adjustable opacity and scale; their light shadows are turned off. Status todo.
- [[task-473-miner-light-calibration]] — The miner light colour, intensity and pool size are tuned in game until the light around the miner matches the warm orange-pink pool of owner image 2. Status todo.
- [[task-475-wall-height-miner]] — Rock, ore and heal ore blocks are raised slightly so their top is at the height of the shrunk miner; play and destruction unchanged. Status todo.
