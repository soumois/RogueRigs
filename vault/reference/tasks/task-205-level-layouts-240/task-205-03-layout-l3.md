---
id: task-205-03-layout-l3
description: "level-design: Write arena_l3 the same way, starting from the rules refined in task-205-02, with a different seed, and report the rules it had to refine.."
parent: task-205-level-layouts-240
kind: task
status: done
---
# layout-l3

- goal: Write arena_l3 the same way, starting from the rules refined in task-205-02, with a different seed, and report the rules it had to refine.
- agent: level-design
- depends_on: task-205-02-layout-l2
- inputs: agent-level-design (vault/reference/agents/agent-level-design.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), barrel (vault/reference/gdd/terrain/barrel.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), Content/RogueRigs/Data/Levels/arena_l2.params.json
- expected_output: Content/RogueRigs/Data/Levels/arena_l3.json, arena_l3.brief.json, arena_l3.params.json; report as in task-205-02.
- done_when: validator exits 0 on arena_l3 with its brief; brief bounds equal arena_m1_fine.brief.json; refined rules recorded; committed.
- editor_state: any
- token_ceiling: 30000
- model: sonnet
- tokens: input 24, cache write 21087, cache read 201050, output 477
- runs: 1
- milestone: M2
- result: arena_l3 (f8ccf4a9): seed 8300, params unchanged from arena_l2, picked among 28 batches; coverage 57.6 %, reach 85.6 %, passage 3, heal 48, barrels 17, 7 pockets, 24 x 24 open 0.07 to 0.94, no dead zone. Rules refined: review 20+ seeds, scan stride, straight-lane tolerance +-1 (layout-review-rules).

## Relations
- [[task-205-level-layouts-240]] — parent global task.
