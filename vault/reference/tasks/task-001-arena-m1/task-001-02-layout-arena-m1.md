---
id: task-001-02-layout-arena-m1
description: "Write arena_m1.json: a 60 x 60 irregular asteroid in void, about 40 percent rock, 6 to 10 heal ore, no barrel, validated."
parent: task-001-arena-m1
kind: task
status: in-progress
---
# Layout: arena_m1

- goal: the fixed M1 arena layout and its brief, passing the validator.
- agent: level-design
- depends_on: task-001-01-validator-void
- inputs: [[level-layout-schema]], [[arena-grid]], [[heal-ore]], [[destruction-rules]].
- expected_output: `Content/RogueRigs/Data/Levels/arena_m1.json` (name `arena_m1`, 60 × 60) and `arena_m1.brief.json`. Intent: an irregular, roughly round asteroid outline (bays and promontories, no straight edge longer than 6 cells); void on every side, with at least 2 void cells between the asteroid and the grid edge; asteroid of about 2000 to 2600 cells; spawn near the center; rock as clusters and short walls that make corridors and open pockets, not noise; heal ore spread over the asteroid, mostly inside or against rock clusters, none within 6 cells of the spawn. Types used: `void`, `rock`, `heal_ore`, `spawn` only (no ore, no barrel). Brief: `rock_coverage` [0.37, 0.43], `counts` heal_ore [6, 10] and barrel [0, 0], `reachable_floor_min` 0.85.
- done_when: `node Tools/validate_layout.js Content/RogueRigs/Data/Levels/arena_m1.json --brief Content/RogueRigs/Data/Levels/arena_m1.brief.json` exits 0; the ASCII preview shows a non-rectangular outline.
- editor_state: any
- token_ceiling: 30000
- milestone: M1
- result: open

## Relations
- [[task-001-arena-m1]] — parent global task.
