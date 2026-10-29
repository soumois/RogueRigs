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
- [[task-309-chest-opening]] — Chest opening: latch swings, lid slams open with overshoot, freeze and shake, inner light, ground ring, three popping XP chips, sparks. Status wait-answers.
- [[task-314-chest-lid-logo]] — Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game. Status todo.
- [[task-368-extractor-rebake]] — Re-unwrap the 47,000-triangle flattened extractor and bake the Meshy high-poly look (colour, normal, roughness, metallic) onto it, fixing the stretched rust marks. Status wait-answers.
- [[task-367-extractor-rotors]] — Cut the four rotors of the flattened extractor mesh into separate meshes pivoting on their spin axes, body kept fixed, ready to spin later; runs after task-368. Status todo.
