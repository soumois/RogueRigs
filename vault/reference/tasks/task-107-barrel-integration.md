---
id: task-107-barrel-integration
description: "Global task: the validated barrel mesh replaces the grid cylinder: C++ fits it to one cell, then BP_ArenaGrid uses it with its materials."
parent: tasks-done-index
kind: task
status: done
---
# Barrel grid scale

- request: intégrer le nouveau mesh du baril dans la map : le BP qui représente le baril est BP_ArenaGrid (BarrelMesh, BarrelMaterial); voie B: calcul d'échelle propre au baril en C++ (owner 2026-10-11) (owner, 2026-10-12)
- goal: The validated barrel mesh replaces the grid barrel cylinder in the arena: C++ fits it to one cell, then BP_ArenaGrid uses it with its materials.
- acceptance: build exits 0 with the editor closed; the RogueRigs.* tests pass headless (existing ones; only fixed if broken by this change); barrels still use the cube transform when BarrelMesh is null; the collision of barrels is unchanged (stated in the report); sys-grid updated; lint clean; committed; committed.
- gdd_check: done by the Orchestrator when the task is planned (2026-10-12).
- confirmed_by_owner: yes (2026-10-12)
- milestone: M2
- next_step: none
- result: Done. SM_Barrel and its textures imported to /Game/RogueRigs/Art/Barrel/ (9 slots in order, slot 0 body, no collision, lightmap index 1), M_BarrelPaint with scratch blend on lid and cap, 7 instances, sticker opaque, warning Masked; BP_ArenaGrid BarrelMesh = SM_Barrel and BarrelMaterial = MI_Barrel_body, level inherits; 16 barrel instances unchanged, uniform scale 0.742; no log error. Open: an unused UV channel 2 from the auto lightmap generation, two MapCheck usage-flag warnings not re-run, no in-game render seen. Commits ab45fb4, d1043f8, 179bb93.

## Subtasks
1. task-107-01-barrel-integration — cpp
2. task-107-02-barrel-integration-bp — bp

## Relations
- [[tasks-done-index]] — parent index.
- [[task-107-01-barrel-integration]] — step 1, cpp.
- [[task-107-02-barrel-integration-bp]] — step 2, bp.
