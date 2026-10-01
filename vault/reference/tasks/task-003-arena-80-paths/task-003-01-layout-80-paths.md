---
id: task-003-01-layout-80-paths
description: "Rewrite arena_m1 at 80 x 80 as organic winding paths walled by rock across the whole asteroid, with hidden pockets to dig into, 60 percent rock."
parent: task-003-arena-80-paths
kind: task
status: in-progress
---
# Layout: arena_m1 80 × 80, organic paths

- goal: an asteroid where the player moves along winding paths between rock walls everywhere, and where digging through a wall can open onto other, hidden paths.
- agent: level-design
- depends_on: none
- inputs: [[level-layout-schema]], [[arena-grid]], [[heal-ore]], [[destruction-rules]], [[task-002-03-layout-rock-60]] (previous version and its constraints).
- expected_output: `Content/RogueRigs/Data/Levels/arena_m1.json` (`width` and `height` 80) and `arena_m1.brief.json` rewritten. Structure: organic paths, mostly 2 to 3 cells wide (width may vary along a path), that curve and wander (no straight path segment longer than about 8 cells), with loops, junctions and dead ends, spread over the whole asteroid up to near the cliff. A main network connected to the spawn, plus at least 4 hidden pockets or path segments (each at least 15 floor cells) not connected to it, separated from a path by 1 to 3 cells of rock, so digging there opens onto them. A few small clearings (at most 8 × 8), one around the spawn. Rock fills the rest; long rock walls following a path are fine. Measured constraints: every asteroid cell within 5 cells of a floor cell; no isolated single rock cell. Kept from before: irregular outline with no straight edge longer than 6 cells; at least 2 void cells to the grid edge; one connected asteroid of 3600 to 4600 cells; spawn near the center (about (40, 40)) with its 5 × 5 area clear; types `void`, `rock`, `heal_ore`, `spawn` only; heal ore none within 6 cells of the spawn, set in walls along paths and inside some hidden pockets, spread over the asteroid. Brief: `width`/`height` 80, `rock_coverage` [0.57, 0.63], `counts` heal_ore [10, 16] and barrel [0, 0], `reachable_floor_min` 0.6.
- done_when: `node Tools/validate_layout.js Content/RogueRigs/Data/Levels/arena_m1.json --brief Content/RogueRigs/Data/Levels/arena_m1.brief.json` exits 0; orchestrator measures: longest straight outline edge ≤ 6, one asteroid piece, every asteroid cell within 5 cells of floor, at least 4 unreachable floor regions of 15+ cells each within 3 cells of reachable floor, reachable floor between 60 % and 90 %.
- editor_state: any
- token_ceiling: 30000
- milestone: M1
- result: open

## Relations
- [[task-003-arena-80-paths]] — parent global task.
