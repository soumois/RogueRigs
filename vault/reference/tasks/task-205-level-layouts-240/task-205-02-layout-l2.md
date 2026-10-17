---
id: task-205-02-layout-l2
description: "level-design: Write arena_l2."
parent: task-205-level-layouts-240
kind: task
status: in-progress
---
# layout-l2

- goal: Write arena_l2: run arena_gen_fine.js with its own parameter file and seed, tune parameters until the layout passes the validator and matches the arena_m1_fine intent (irregular asteroid, organic 3+ cell paths, hidden pockets, ore clusters, 12 heal veins, 14 to 18 barrels), and report the rules it had to refine.
- agent: level-design
- depends_on: task-205-01-direct-240-generator
- inputs: agent-level-design (vault/reference/agents/agent-level-design.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), barrel (vault/reference/gdd/terrain/barrel.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), Content/RogueRigs/Data/Levels/arena_m1_fine.brief.json
- expected_output: Content/RogueRigs/Data/Levels/arena_l2.json, arena_l2.brief.json, arena_l2.params.json; report with counts, coverage, reachable floor, ASCII preview (downscaled) and refined rules.
- done_when: node Tools/validate_layout.js Content/RogueRigs/Data/Levels/arena_l2.json --brief Content/RogueRigs/Data/Levels/arena_l2.brief.json exits 0; brief bounds equal arena_m1_fine.brief.json; the orchestrator records the refined rules in layout-generator-recipe or agent-level-design; committed.
- editor_state: any
- token_ceiling: 30000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-205-level-layouts-240]] — parent global task.
