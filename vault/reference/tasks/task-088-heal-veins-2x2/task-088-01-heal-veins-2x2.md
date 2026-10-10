---
id: task-088-01-heal-veins-2x2
description: "Heal veins are 2 x 2 in the layout and in code: 12 veins, 15 HP per pickup, 60 HP per vein."
parent: task-088-heal-veins-2x2
kind: task
status: in-progress
---
# Heal veins 2 x 2 (layout)

- goal: Heal veins are 2 x 2 in the layout and in code: 12 veins, 15 HP per pickup, 60 HP per vein.
- agent: level-design
- depends_on: none
- inputs: heal-ore (vault/reference/gdd/terrain/heal-ore.md), arena-grid (vault/reference/gdd/terrain/arena-grid.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), readability-rules (vault/reference/gdd/art-audio/readability-rules.md)
- expected_output: regenerate the heal veins of the fine layout (arena_m1_fine, and the base layout it derives from if the recipe requires it) as 2 x 2 clusters: keep 12 veins (48 heal cells instead of 108), same positions as the former 3 x 3 veins where they fit, never on barrels or console-marked rocks; validate with Tools/validate_layout.js; no other layout change; document in the layout notes. A cpp subtask (task-088-02) then makes the vein size and the 15 HP per pickup follow the data.
- done_when: validate_layout.js exits 0; 12 heal veins of 2 x 2 (48 cells) counted by script; layout notes updated; lint clean; committed.
- editor_state: any
- token_ceiling: 50000
- model: open
- tokens: open
- runs: open
- milestone: M2
- result: open

## Relations
- [[task-088-heal-veins-2x2]] — parent global task.
