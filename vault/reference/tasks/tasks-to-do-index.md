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
- [[task-188-editor-lock]] — Editor lock shared by sessions: a lock file, a node script to take, release and read it, and the runbook and protocol rules to use it. Status todo.
- [[task-230-rock-slab-integration]] — Dual-grid slab rock look in the grid: pieces, world-UV top texture, cracks, plates, crystals. Status in-progress.
- [[task-234-ore-deposit-look]] — Each 2 x 2 ore or heal deposit is drawn as one crystal formation, at least 3 variants per type, from images the owner generates with prompts we write. Status in-progress.
- [[task-250-fix-heal-vein-test]] — RogueRigs.Ore.HealVein passes again: find why only 3 of 4 heal pickups are collected (health 55 instead of 70) and fix the cause, in the game if it breaks the heal ore rule, in the test if only its expectation is outdated. Status todo.
- [[task-253-drone-parts-animation]] — The drone has three animated parts: the antenna spins, the base turns slowly toward the mining spot and the laser pivots in height. Status in-progress.
