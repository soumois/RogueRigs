---
id: task-205-01-direct-240-generator
description: "cpp: Write Tools/layout_gen/arena_gen_fine.js."
parent: task-205-level-layouts-240
kind: task
status: done
---
# direct-240-generator

- goal: Write Tools/layout_gen/arena_gen_fine.js: the layout-generator-recipe run natively at 240 x 240 (outline, wander walks, pockets, filler, cleanup, heal veins 2 x 2, barrels, 3-cell passage rule, seed search), every parameter in a JSON parameter file, with CLI arguments --params --seed --name --out --brief.
- agent: cpp
- depends_on: none
- inputs: layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), agent-cpp (vault/reference/agents/agent-cpp.md), Tools/layout_gen/*.js, Tools/validate_layout.js, Content/RogueRigs/Data/Levels/arena_m1_fine.brief.json
- expected_output: Tools/layout_gen/arena_gen_fine.js, Tools/layout_gen/params/fine_default.json (starting parameters: the 80 x 80 ones scaled by 3 where they are lengths), a "Direct 240 generation" section in layout-generator-recipe.
- done_when: running it with the default parameters and two different seeds writes two layouts to a scratch folder that pass node Tools/validate_layout.js with arena_m1_fine.brief.json; same seed twice gives byte-identical output; the existing scripts and Content/RogueRigs/Data/Levels/ are unchanged; recipe doc updated; committed.
- editor_state: any
- token_ceiling: 60000
- model: opus
- tokens: input 52, cache write 87969, cache read 1490989, output 15325
- runs: 1
- milestone: M2
- result: Tools/layout_gen/arena_gen_fine.js + params/fine_default.json (3b396e76): recipe native at 240 x 240, lengths x3, areas x9, CLI --params --seed --name --out --brief with seed search; seeds 3 and 100 pass the validator with the arena_m1_fine brief, deterministic. Rule changes: barrels on reachable floor against a wall, heal veins placed directly 2 x 2, looped passage repair, rock islands <= 9 cells to floor, outline jitter per seed; shortest-corridor ranking not ported. Not loaded in Unreal.

## Relations
- [[task-205-level-layouts-240]] — parent global task.
- [[layout-generator-recipe]] — its "Direct 240 generation" section documents the generator.
