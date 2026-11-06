---
id: task-472-01-shadow-component
description: "Cpp: One generic shadow component placed under its actor, following it, with per-type and global opacity and scale, and the listed actors stop casting light shadows."
parent: task-472-actor-blob-shadows
kind: task
status: done
---
# Light-independent purple-grey shadows under actors: cpp

- goal: One generic shadow component placed under its actor, following it, with per-type and global opacity and scale, and the listed actors stop casting light shadows.
- agent: cpp
- depends_on: none
- inputs: visual-style (vault/reference/gdd/art-audio/visual-style.md), color-code (vault/reference/gdd/art-audio/color-code.md), task-264-blob-shadows (vault/reference/tasks/task-264-blob-shadows.md)
- expected_output: see done_when
- done_when: A1, A2, A5 (settings exposed), A7 and A9 hold; tests for placement, follow and settings pass; committed.
- editor_state: decided by the Executor
- token_ceiling: open
- model: opus
- tokens: included in the executor run of task-472
- usage: included in the executor run of task-472
- runs: 1
- milestone: M2
- started: 2026-11-06T17:53:44Z
- waiting_for: none
- result: Done (890894ef). URRBlobShadowComponent: per-type Opacity 0.6 and SizeScale 1.2 on the component (Blueprint defaults), footprint x SizeScale x global; opacity written to custom primitive data 0 for the material. Global multipliers: URRBlobShadowSettings (Project Settings > Game > Actor Shadows, GlobalOpacityMultiplier, GlobalScaleMultiplier); needed the DeveloperSettings module in RogueRigs.Build.cs (one line). Miner and robots footprint = capsule diameter; drone footprint 70, fixed size at every height (Q3); turret and bonus chest fitted to their meshes at BeginPlay (FitToOwnerMeshes), chest casts no light shadow; one shadow per barrel built by ARRArenaGrid (BarrelShadowOpacity, BarrelShadowScale), destroyed with the barrel; barrel HISMs cast no shadow. Build OK; RogueRigs.BlobShadow (3 new tests: settings, under miner and robot and follow, barrels), Barrel, BarrelFuse, Turret, Enemies, Player, Console: 40 of 40 pass. Consoles keep no shadow (Q1).

## Relations
- [[task-472-actor-blob-shadows]] — parent global task.
