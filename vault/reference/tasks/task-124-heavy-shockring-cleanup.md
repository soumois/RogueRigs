---
id: task-124-heavy-shockring-cleanup
description: "Global task: The hidden ShockRing component of BP_Heavy is deleted: its test lookups are removed in C++, then the component is removed from the Blueprint."
parent: tasks-done-index
kind: task
status: done
---
# Delete the heavy ShockRing component

- request: Créer la tâche de suppression du composant ShockRing (cache) du heavy (owner, 2026-10-13)
- goal: The hidden ShockRing component of BP_Heavy is deleted: its test lookups are removed in C++, then the component is removed from the Blueprint.
- acceptance: build exits 0 with the editor closed; RogueRigs.Integration.HeavyTelegraphSlamsMiner and RogueRigs.Heavy.* pass headless; grep finds no ShockRing in Source/; sys-tests-heavy updated; lint clean; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-13).
- confirmed_by_owner: yes (2026-10-13, launched by the owner)
- milestone: M2
- next_step: none
- result: Done. ShockRing component removed from BP_Heavy, compiled and saved, read back; commit 36483d2

## Subtasks
1. task-124-01-heavy-shockring-cleanup — cpp
2. task-124-02-remove-shockring-component — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-124-01-heavy-shockring-cleanup]] — step 1, cpp.
- [[task-124-02-remove-shockring-component]] — step 2, bp.
