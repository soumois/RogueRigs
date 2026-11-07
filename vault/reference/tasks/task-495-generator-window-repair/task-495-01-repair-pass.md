---
id: task-495-01-repair-pass
description: "Cpp: Window repair pass in FRRLayoutGenerator (carve into dead windows, add rock into too-open windows) plus the window check in seed acceptance, keeping every recipe and validator constraint and determinism."
parent: task-495-generator-window-repair
kind: task
status: todo
---
# Repair pass so every generated layout meets the 24 x 24 window rule: cpp

- goal: Window repair pass in FRRLayoutGenerator (carve into dead windows, add rock into too-open windows) plus the window check in seed acceptance, keeping every recipe and validator constraint and determinism.
- agent: cpp
- depends_on: none
- inputs: sys-layout-generator (vault/reference/tech/systems/world/sys-grid/sys-layout-generator.md), layout-generator-recipe (vault/reference/tech/pipelines/layout-generator-recipe.md), layout-review-rules (vault/reference/tech/pipelines/layout-review-rules.md), level-layout-schema (vault/reference/tech/pipelines/level-layout-schema.md), Saved/GeneratedLevels/task-492/review_cpp.md
- expected_output: see done_when
- done_when: A1, A2, A3, A5 and A6 met on the 24 starts of review_cpp.md, measured by the Executor (window min and max per layout, validator output, two-run hash, retries and times) and written in the subtask result; carved floor joins the main network (no new isolated region) and added rock keeps passage 3.
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
- [[task-495-generator-window-repair]] — parent global task.
