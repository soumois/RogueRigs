---
id: task-029-03-layout-67
description: "L_ArenaM1 layout converted to 240 × 240 cells with organic edges on rock masses, passages at least 3 cells wide, validator updated and passing."
parent: task-029-fine-grid
kind: task
status: done
---
# Layout at 67 uu

- goal: the M1 arena exists at fine resolution.
- agent: level-design
- depends_on: task-029-02-flow-field-incremental
- inputs: [[arena-grid]], [[level-layout-schema]], [[layout-generator-recipe]], current arena_m1 layout JSON, [[drgs-analysis]]
- expected_output: new layout file for 240 × 240 (each former cell becomes 3 × 3, then rock mass borders roughened by a deterministic rule so edges are irregular, ore and heal veins kept as clusters, barrels as single cells near their former position, spawn kept), passages ≥ 3 cells, validator extended for the size and the minimum passage width and passing; the old 80 × 80 layout kept for tests; level-layout-schema and arena-grid notes updated.
- done_when: validator passes on the new layout; lint clean; committed.
- editor_state: closed
- token_ceiling: 90000
- model: sonnet
- tokens: input 28, cache write 54518, cache read 461419, output 892
- runs: 1
- milestone: M2
- result: arena_m1_fine.json 240 × 240 (commit 4964905): 3 × 3 conversion, deterministic roughening (value noise, seed 6701), validator extended with min_passage_width; OK, min passage width 3, rock 17904, heal_ore 108 (12 clusters of 9), barrels 16, reachable floor 85.5 %. Brief bounds rock_coverage and barrels chosen by the agent. Old 80 × 80 layout kept.

## Relations
- [[task-029-fine-grid]] — parent global task.
