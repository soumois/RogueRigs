---
id: task-003-01-layout-80-paths
description: "Rewrite arena_m1 at 80 x 80 as a network of 2-3 cell paths walled by rock across the whole asteroid, 60 percent rock."
parent: task-003-arena-80-paths
kind: task
status: todo
---
# Layout: arena_m1 80 × 80, path network

- goal: an asteroid where the player moves along paths between rock walls everywhere, not in one open center surrounded by a thick rock ring.
- agent: level-design
- depends_on: none
- inputs: [[level-layout-schema]], [[arena-grid]], [[heal-ore]], [[destruction-rules]], [[task-002-03-layout-rock-60]] (previous version and its constraints).
- expected_output: `Content/RogueRigs/Data/Levels/arena_m1.json` (`width` and `height` 80) and `arena_m1.brief.json` rewritten. Structure: paths 2 to 3 cells wide forming a connected network with loops and junctions (several routes between two points, few dead ends), spread evenly over the asteroid up to near the cliff; a few small clearings (at most 8 × 8), one of them around the spawn; rock fills the space between paths as walls 2 to 6 cells thick. Measured constraints: every asteroid cell within 5 cells of a floor cell; no solid rock square larger than 6 × 6; no isolated single rock cell. Kept from before: irregular outline with no straight edge longer than 6 cells; at least 2 void cells to the grid edge; one connected asteroid of 3600 to 4600 cells; spawn near the center (about (40, 40)) with its 5 × 5 area clear; types `void`, `rock`, `heal_ore`, `spawn` only; heal ore none within 6 cells of the spawn, placed in walls along paths, spread over the asteroid. Brief: `width`/`height` 80, `rock_coverage` [0.57, 0.63], `counts` heal_ore [10, 16] and barrel [0, 0], `reachable_floor_min` 0.9.
- done_when: `node Tools/validate_layout.js Content/RogueRigs/Data/Levels/arena_m1.json --brief Content/RogueRigs/Data/Levels/arena_m1.brief.json` exits 0; orchestrator measures: longest straight edge ≤ 6, one piece, largest solid rock square ≤ 6, every asteroid cell within 5 cells of floor.
- editor_state: any
- token_ceiling: 30000
- milestone: M1
- result: open

## Relations
- [[task-003-arena-80-paths]] — parent global task.
