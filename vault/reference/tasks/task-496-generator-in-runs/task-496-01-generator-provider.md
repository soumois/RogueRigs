---
id: task-496-01-generator-provider
description: "Cpp: A run layout provider that generates each level with the C++ generator from a per-run seed, per-level params and brief from packaged content, hand JSON fallback."
parent: task-496-generator-in-runs
kind: task
status: todo
---
# Layout generator wired into real runs: cpp

- goal: A run layout provider that generates each level with the C++ generator from a per-run seed, per-level params and brief from packaged content, hand JSON fallback.
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), sys-run-layout (vault/reference/tech/systems/world/sys-run/sys-run-layout.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), task-492-cpp-layout-generator (vault/reference/tasks/task-492-cpp-layout-generator.md), task-493-async-layout-loader (vault/reference/tasks/task-493-async-layout-loader.md), task-494-generator-test-map (vault/reference/tasks/task-494-generator-test-map.md), Content/RogueRigs/Data/Levels/, Tools/layout_gen/params/fine_default.json
- expected_output: see done_when
- done_when: Acceptance A1, A4, A5 and the seed part of A6 hold in a -game -nullrhi log run through 4 levels (rr.Run.NextLevel or the existing run flow); determinism checked by two generations of one seed; test map and commandlet still work; build clean, existing tests that the change breaks updated.
- editor_state: decided by the Executor
- token_ceiling: open
- model: open
- tokens: open
- usage: open
- runs: open
- milestone: M2
- started: open
- waiting_for: none
- result: open

## Relations
- [[task-496-generator-in-runs]] — parent global task.
