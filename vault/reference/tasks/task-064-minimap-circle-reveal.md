---
id: task-064-minimap-circle-reveal
description: "Global task: Minimap reveal is a circle of 18 cells around the miner, the same in every direction."
parent: tasks-done-index
kind: task
status: done
---
# Circular minimap reveal

- request: "lorsque j’arrive en jeu et que je n’ai pas bougé, la zone devrait être ronde, pas un trapèze. En plus, j’ai l’impression que je découvre plus vite la map vers le haut que vers le bas" (owner, 2026-10-07)
- goal: Minimap reveal is a circle of 18 cells around the miner, the same in every direction.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: none
- result: Done. URRMinimap reveal: circle of RevealRadiusCells 18 (editable) around the miner cell, checked only when the miner changes cell (about 1000 cell tests), replacing the perspective view trapezoid; Tick takes the reveal actor. No test changed. Build 0; full run 193/195 (known BudgetAt67uu and PressureHordeBanner). Commit b53047b.

## Subtasks
1. task-064-01-minimap-circle-reveal — cpp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-064-01-minimap-circle-reveal]] — step 1, cpp.
