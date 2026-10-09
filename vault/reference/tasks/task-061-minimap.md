---
id: task-061-minimap
description: "Global task: Round minimap at the top left: seen cells of rock, floor and heal veins, markers for miner, console, chest and shuttle."
parent: tasks-index
kind: task
status: todo
---
# Minimap

- request: "ajouter la mini carte" (owner, 2026-10-07)
- goal: Round minimap at the top left: seen cells of rock, floor and heal veins, markers for miner, console, chest and shuttle.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-061-01-minimap
- result: open

## Subtasks
1. task-061-01-minimap — cpp
2. task-061-02-bp-minimap — bp

## Relations
- [[tasks-index]] — parent index.
- [[task-061-01-minimap]] — step 1, cpp.
- [[task-061-02-bp-minimap]] — step 2, bp.
