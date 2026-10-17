---
id: task-205-level-layouts-240
description: "Global task: Level-Design writes the level 2 to 4 layouts directly at 240 x 240 and refines its rules."
parent: tasks-to-do-index
kind: task
status: in-progress
---
# Level layouts 2 to 4 at 240 x 240

- request: "On va pour l'instant génerer les niveaux via Claude ( agent level designer ), comme ça, ça nous permettra aussi d'affiner ses régles et être sur que cela n'était pas un coup de chance d'avoir un bon terrain." Then: "est-ce nécessaire de passer par le 80x80 ? On aura quelques choses de plus organiques en 240x240, non ?" and "Garde le Niveau 1 pour l'instant" (owner, 2026-10-16 and 2026-10-17)
- goal: the Level-Design agent writes the layouts of levels 2 to 4 directly at 240 x 240, refining its rules so a good terrain is not luck.
- acceptance: arena_l2, arena_l3, arena_l4 exist in Content/RogueRigs/Data/Levels/, each passes node Tools/validate_layout.js with its brief (same bounds as arena_m1_fine); generated directly at 240 x 240 by a deterministic Node tool driven by a parameter file; arena_m1_fine and the old tools output unchanged; refined rules recorded in layout-generator-recipe and agent-level-design; ASCII previews shown to the owner; lint clean; committed.
- gdd_check: [[arena-grid]] (approved; updated 2026-10-17: Level-Design writes each run level at 240 x 240 beforehand until the C++ generator exists), [[layout-generator-recipe]], [[level-layout-schema]]. Owner 2026-10-17: generate directly at 240 x 240 (more organic than 80 x 80 upscaled), same brief as arena_m1_fine, level 1 stays arena_m1_fine.
- confirmed_by_owner: yes (2026-10-17)
- milestone: M2
- next_step: task-205-02-layout-l2
- result: open

## Subtasks
1. task-205-01-direct-240-generator — cpp
2. task-205-02-layout-l2 — level-design
3. task-205-03-layout-l3 — level-design
4. task-205-04-layout-l4 — level-design

## Relations
- [[tasks-to-do-index]] — parent index.
- [[task-205-01-direct-240-generator]] — step 1, cpp.
- [[task-205-02-layout-l2]] — step 2, level-design.
- [[task-205-03-layout-l3]] — step 3, level-design.
- [[task-205-04-layout-l4]] — step 4, level-design.
