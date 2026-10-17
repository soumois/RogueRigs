---
id: task-230-rock-slab-integration
description: "Integrate the dual-grid slab rock look (pieces, world-UV top texture, cracks, plates, crystals) in the grid; prepared, not launched."
parent: tasks-to-do-index
kind: task
status: todo
---
# Rock slab look: integration

- request: "Oui : grille duale pour l'affichage, budget de triangles provisoire, liseré dans le même matériau. Prépare la tâche d'intégration sans la lancer." (owner, 2026-10-17)
- goal: the arena rock is drawn as continuous slabs with a rounded organic outline (dual grid), a world-UV top texture, cracks as soon as a block is below 100 %, tilted plates on heavy damage and crystal clusters on ore and heal cells; gameplay unchanged.
- acceptance: every subtask done; RogueRigs.* tests pass; after captures compared with before and sent to the owner; perf measured against the baseline and the provisional budget; docs updated; lint clean; committed.
- gdd_check: [[rock-state-render-options]] and children (owner decisions 2026-10-17: dual grid for display only, provisional triangle budget, rim as vertex colour in one material, cracks from any damage, one variant per shape with variant support, Nanite assumed), [[art-style-guide]] A1 level 3, [[color-code]], [[readability-rules]], [[sys-grid]]. Open: weak rock (separate task, maximum share open), number of variants later. Not in this task: weak rock, collision rounding of corners.
- confirmed_by_owner: no (prepared on request, launch waits for the owner)
- milestone: M2
- next_step: task-230-01-rock-baseline-capture
- result: open

## Subtasks
1. task-230-01-rock-baseline-capture — cpp: Baseline before the change: -game captures of rock areas and a grid perf measure.
2. task-230-02-rock-slab-pieces — 3d: Dual-grid slab piece meshes: full, edge, outer corner, inner corner, diagonal; Blender script.
3. task-230-03-rock-top-texture — art: Tileable painted rock top texture for the world-UV slab material.
4. task-230-04-rock-top-texture-review — style-guide: Review of the rock top texture candidates.
5. task-230-05-rock-crack-mask — 3d: Procedural tileable crack mask (grayscale) whose value orders crack growth from 0 to 1.
6. task-230-06-rock-plate-overlay — 3d: Broken-top overlay mesh: cell-sized plates tilted up, shown on heavily damaged cells.
7. task-230-07-dual-grid-placement — cpp: ARRArenaGrid draws rock with dual-grid pieces at cell corners and rebuilds them on destruction.
8. task-230-08-rock-damage-data — cpp: Damage display data: per-quadrant custom data, shake in the material, plate overlay and ore crystal HISMs.
9. task-230-09-rock-slab-material — bp: Material M_RockSlab and its instance: world-UV top, sides, rim, cracks, shake.
10. task-230-10-heal-crystal-material — bp: Red heal crystal material instance for the crystal clusters on heal veins.
11. task-230-11-rock-slab-assign — bp: Import pieces, plates and textures; assign meshes and materials in BP_ArenaGrid.
12. task-230-12-rock-slab-check — cpp: After the change: -game captures at the baseline spots and the grid perf measure, compared with step 01.

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-230-01-rock-baseline-capture]] — step 1, cpp.
- [[task-230-02-rock-slab-pieces]] — step 2, 3d.
- [[task-230-03-rock-top-texture]] — step 3, art.
- [[task-230-04-rock-top-texture-review]] — step 4, style-guide.
- [[task-230-05-rock-crack-mask]] — step 5, 3d.
- [[task-230-06-rock-plate-overlay]] — step 6, 3d.
- [[task-230-07-dual-grid-placement]] — step 7, cpp.
- [[task-230-08-rock-damage-data]] — step 8, cpp.
- [[task-230-09-rock-slab-material]] — step 9, bp.
- [[task-230-10-heal-crystal-material]] — step 10, bp.
- [[task-230-11-rock-slab-assign]] — step 11, bp.
- [[task-230-12-rock-slab-check]] — step 12, cpp.
- [[rock-state-render-options]] — the design this task implements.
- [[task-219-rock-ore-look]] — the look exploration behind it.
