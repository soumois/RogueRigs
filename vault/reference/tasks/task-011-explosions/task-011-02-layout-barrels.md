---
id: task-011-02-layout-barrels
description: "Add about 16 barrel cells to arena_m1 in walls along paths, some in pairs, without changing rock, heal ore or paths."
parent: task-011-explosions
kind: task
status: todo
---
# Layout: barrels

- goal: barrels to shoot in the arena.
- agent: level-design
- depends_on: none
- inputs: [[level-layout-schema]], [[layout-generator-recipe]], [[arena-grid]], [[barrel]], [[destruction-rules]].
- expected_output: `Content/RogueRigs/Data/Levels/arena_m1.json` with 14 to 18 rock cells turned into `barrel`: each touches the main reachable floor (shootable from a path), none within 6 cells of the spawn, spread over the asteroid, 3 to 4 of them in pairs (two barrels at most 2 cells apart, for chains); no other cell changes. Brief `arena_m1.brief.json`: `counts` barrel [14, 18], everything else unchanged (rock coverage counts barrels as blocks). Edit only the two files; report the barrel cells.
- done_when: validator exits 0 with the brief; orchestrator diff shows only rock → barrel changes.
- editor_state: any
- token_ceiling: 30000
- milestone: M1
- result: open

## Relations
- [[task-011-explosions]] — parent global task.
