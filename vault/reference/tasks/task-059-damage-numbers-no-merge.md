---
id: task-059-damage-numbers-no-merge
description: "Global task: Damage numbers: one per hit, no merging, each starting at a random point of a 30 uu disc (45 uu on the heavy); stacking kept."
parent: tasks-index
kind: task
status: todo
---
# Damage numbers without merging

- request: "ne plus fusionner les chiffres de dégâts. Il devrait avoir un radius de spawn sur le monstre" (owner, 2026-10-07)
- goal: Damage numbers: one per hit, no merging, each starting at a random point of a 30 uu disc (45 uu on the heavy); stacking kept.
- acceptance: build exits 0 with the editor closed; all `RogueRigs*` tests pass headless; docs updated; lint clean.; committed.
- gdd_check: GDD updated by the Orchestrator (owner 2026-10-07, fifth test). No open value.
- confirmed_by_owner: yes (2026-10-07)
- milestone: M2
- next_step: task-059-01-damage-numbers-no-merge
- result: open

## Subtasks
1. task-059-01-damage-numbers-no-merge — cpp

## Relations
- [[tasks-index]] — parent index.
- [[task-059-01-damage-numbers-no-merge]] — step 1, cpp.
