---
id: task-107-barrel-integration
description: "Global task: the validated barrel mesh replaces the grid cylinder: C++ fits it to one cell, then BP_ArenaGrid uses it with its materials."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Barrel grid scale

- request: intégrer le nouveau mesh du baril dans la map : le BP qui représente le baril est BP_ArenaGrid (BarrelMesh, BarrelMaterial); voie B: calcul d'échelle propre au baril en C++ (owner 2026-10-11) (owner, 2026-10-12)
- goal: The validated barrel mesh replaces the grid barrel cylinder in the arena: C++ fits it to one cell, then BP_ArenaGrid uses it with its materials.
- acceptance: build exits 0 with the editor closed; the RogueRigs.* tests pass headless (existing ones; only fixed if broken by this change); barrels still use the cube transform when BarrelMesh is null; the collision of barrels is unchanged (stated in the report); sys-grid updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: task-107-01-barrel-integration
- result: open

## Subtasks
1. task-107-01-barrel-integration — cpp
2. task-107-02-barrel-integration-bp — bp

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-107-01-barrel-integration]] — step 1, cpp.
- [[task-107-02-barrel-integration-bp]] — step 2, bp.
