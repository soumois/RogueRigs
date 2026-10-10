---
id: task-041-front-detour
description: "Global task: In the front sector the spawn detour filter is off, so robots spawn beyond the rock the miner digs toward."
parent: tasks-done-index
kind: task
status: done
---
# Spawns beyond the rock ahead

- request: "je suis en train de creuser vers le bas, mais aucun ennemi ne m'attend de l'autre coté du chemin [...] il doit en avoir qui pop un peu de ce coté là" (owner, 2026-10-07)
- goal: In the front sector the spawn detour filter is off, so robots spawn beyond the rock the miner digs toward.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. Front-sector cells exempt from MaxPathDetourRatio while running (side, back and still unchanged); cause confirmed by reading: cells behind rock exceeded the 1.5 ratio. Limit: rock thicker than the 100–500 uu band leaves no floor ahead to spawn on. Build 0, 195/195. Commit 6a1f3bd.

## Subtasks
1. task-041-01-front-detour — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-041-01-front-detour]] — step 1, cpp.
