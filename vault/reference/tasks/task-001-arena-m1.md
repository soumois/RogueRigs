---
id: task-001-arena-m1
description: "Global task: the M1 arena as an irregular asteroid with rocks and heal ore, from layout JSON to a grey-box map visible in the editor."
parent: tasks-index
kind: task
status: in-progress
---
# M1 arena, visible in the editor

- request: "Crée l'arène de M1 : du vide autour, quelques minerais de soin et des rochers, quelque chose de simple." Owner answers: heal ore behaves like a block to break and collect but heals nothing for now; rock coverage 40 %; the arena must not be rectangular (it must read as an asteroid); JSON plus an arena visible in the editor; no barrels; test assets built from cubes and colors.
- goal: a map `L_ArenaM1` that opens in the editor and shows the 60 × 60 M1 layout as an irregular asteroid floating in void, built in grey box from the layout JSON by a C++ grid actor.
- acceptance: (1) `node Tools/validate_layout.js Content/RogueRigs/Data/Levels/arena_m1.json --brief Content/RogueRigs/Data/Levels/arena_m1.brief.json` exits 0; (2) the C++ build exits 0 with the editor closed; (3) `L_ArenaM1` opens and shows an irregular floor outline with void around it, grey-violet rock blocks covering about 40 % of the asteroid, 6 to 10 red heal ore blocks, no barrel; (4) in PIE the template character spawns on the spawn cell and walks on the floor; (5) lint clean, every subtask committed.
- gdd_check: read [[arena-grid]], [[heal-ore]], [[destruction-rules]], [[milestones]], [[color-code]], [[level-layout-schema]], [[sys-grid]]. Conflict: heal ore was M2 → owner moves the block to M1; its heal effect stays open. Open values: heal ore density → 6 to 10; rock coverage → 40 % of the asteroid. Arena shape: the schema had no way to mark void → new cell type `void` (owner approved). Grey-box block height and floor color are not in the GDD: provisional values in subtasks 03 and 04. Heal pickups need block destruction, which no system implements yet: out of this task.
- confirmed_by_owner: yes
- milestone: M1
- next_step: task-001-06-map-arena-m1
- result: open

## Subtasks
1. task-001-01-validator-void — cpp: `void` cell type, coverage over the asteroid, floor reachability, in the validator and schema doc.
2. task-001-02-layout-arena-m1 — level-design: `arena_m1.json` and its brief.
3. task-001-03-arena-grid-cpp — cpp: `ARRArenaGrid` loads the JSON and builds floor and blocks with HISM in the editor.
4. task-001-04-greybox-materials — bp: grey-box base material and colored material instances.
5. task-001-05-bp-arena-grid — bp: `BP_ArenaGrid` with layout name, cube mesh and materials, plus its doc node.
6. task-001-06-map-arena-m1 — bp: map `L_ArenaM1` with the grid, lights, player start and game mode.

## Relations
- [[tasks-index]] — parent index.
- [[task-001-01-validator-void]] — step 1, cpp, validator and schema.
- [[task-001-02-layout-arena-m1]] — step 2, level-design, the layout.
- [[task-001-03-arena-grid-cpp]] — step 3, cpp, the grid actor.
- [[task-001-04-greybox-materials]] — step 4, bp, grey-box materials.
- [[task-001-05-bp-arena-grid]] — step 5, bp, the grid Blueprint.
- [[task-001-06-map-arena-m1]] — step 6, bp, the map.
