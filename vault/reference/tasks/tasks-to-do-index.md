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
- [[task-308-chest-lock-light]] — Chest lock light: red blinking while waiting, orange blinking faster with hack progress in the zone, steady green once open. Status todo.
- [[task-309-chest-opening]] — Chest opening: latch swings, lid slams open with overshoot, freeze and shake, inner light, ground ring, three popping XP chips, sparks. Status todo.
- [[task-314-chest-lid-logo]] — Add the yellow Kessler-Voss mark on top of the chest lid, as on the validated sheet, and show it in game. Status todo.
- [[task-334-near-asteroids]] — Other low-poly asteroids floating in the void around the arena asteroid, nearer the camera than the space layers, with depth parallax. Status todo.
- [[task-335-space-texture-layers]] — Replace the procedural nebula of the void background with layered ChatGPT-generated tileable space textures at several depths, procedural stars kept. Status wait-answers.
