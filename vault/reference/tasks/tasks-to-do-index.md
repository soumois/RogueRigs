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
- [[task-336-heal-crystal-variants]] — Images of 3 red heal crystal variants with near-identical silhouettes, visual only, so heal drops do not always look the same. Status wait-answers.
- [[task-337-main-menu-background]] — Main menu background image: one 16:9 scene in the validated key art style, generated through Kie.ai, picked by the owner. Status todo.
- [[task-338-main-menu-screen]] — Main menu screen at game start: background image, New Run starts a run, Options placeholder, Exit quits the game. Status todo.
- [[task-339-heal-chunk-mesh]] — Blender meshes of the 3 red heal pickup chunk variants from the chosen sheet, then in game on BP_PickupHeal, drawn at random per drop. Status todo.
