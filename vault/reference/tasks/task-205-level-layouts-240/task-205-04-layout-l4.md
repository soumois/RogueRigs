---
id: task-205-04-layout-l4
description: "level-design: Write arena_l4 the same way, starting from the rules refined in task-205-03, with a different seed, and report the rules it had to refine.."
parent: task-205-level-layouts-240
kind: task
status: done
---
# layout-l4

- goal: Write arena_l4 the same way, starting from the rules refined in task-205-03, with a different seed, and report the rules it had to refine.
- agent: level-design
- depends_on: task-205-03-layout-l3
- inputs: agent-level-design (vault/reference/agents/agent-level-design.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), destruction-rules (vault/reference/gdd/terrain/destruction-rules.md), ore-blocks (vault/reference/gdd/terrain/ore-blocks.md), heal-ore (vault/reference/gdd/terrain/heal-ore.md), barrel (vault/reference/gdd/terrain/barrel.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), Content/RogueRigs/Data/Levels/arena_l3.params.json
- expected_output: Content/RogueRigs/Data/Levels/arena_l4.json, arena_l4.brief.json, arena_l4.params.json; report as in task-205-02.
- done_when: validator exits 0 on arena_l4 with its brief; brief bounds equal arena_m1_fine.brief.json; refined rules recorded; committed.
- editor_state: any
- token_ceiling: 30000
- model: sonnet
- tokens: input 20, cache write 18595, cache read 154291, output 383
- runs: 1
- milestone: M2
- result: arena_l4 (0f6e6663): seed 10090, noise_seed 9100 and new outline harmonic phases for a different shape; coverage 56.5 %, reach 84.2 %, passage 3, heal 48, barrels 17, 7 pockets, 24 x 24 open 0.12 to 0.93; only 1 of 30 passing seeds passed the window review; straight left lane of about 44 cells accepted (flagged). Rules recorded in layout-review-rules.

## Relations
- [[task-205-level-layouts-240]] — parent global task.
